#include "Component/CombatComponent.h"
#include "Character/NecPlayerCharacter.h"
#include "Component/StaminaComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Item/Weapon_Item_Base.h"
#include "Component/PlayerMovementComponent.h"
#include "DataAsset/WeaponDataAsset.h"
#include "DamageType/NecDamageType.h"

UCombatComponent::UCombatComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

    SetIsReplicatedByDefault(true);
}

void UCombatComponent::BeginPlay()
{
    Super::BeginPlay();

    OwnerCharacter = Cast<ANecPlayerCharacter>(GetOwner());
    if (OwnerCharacter && OwnerCharacter->HasAuthority() && UnarmedWeaponClass)
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = OwnerCharacter;
        SpawnParams.Instigator = OwnerCharacter;

        UnarmedWeaponInstance = GetWorld()->SpawnActor<AWeapon_Item_Base>(UnarmedWeaponClass, SpawnParams);

        if (!CurrentWeapon)
        {
            EquipWeapon(nullptr);
        }
    }
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UCombatComponent, bIsGuarding);
    DOREPLIFETIME(UCombatComponent, CurrentWeapon);
    DOREPLIFETIME(UCombatComponent, UnarmedWeaponInstance);
}

void UCombatComponent::Attack()
{
    if (!OwnerCharacter || !CurrentWeapon)
    {
        return;
    }

    bool bIsMoving = OwnerCharacter->GetVelocity().Size() > 5.0f;

    if (OwnerCharacter->GetPlayerMovementComponent()->GetIsSprinting() && !bIsAttacking && bIsMoving)
    {
        bIsAttacking = true;
        bSaveAttackInput = false;
        bComboWindowOpen = false;
        CurrentComboIndex = 0;

        PlayRunningAttack();
        return;
    }

    if (bIsAttacking)
    {
        if (bComboWindowOpen)
        {
            bSaveAttackInput = true;
        }
    }
    else
    {
        UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
        if (AnimInstance && AnimInstance->Montage_IsPlaying(CurrentWeapon->GetAttackMontage()))
        {
            return;
        }

        bIsAttacking = true;
        bSaveAttackInput = false;
        bComboWindowOpen = false;
        CurrentComboIndex = 0;

        PlayComboAttack();
    }
}

void UCombatComponent::EquipWeapon(AWeapon_Item_Base* NewWeapon)
{
    if (!OwnerCharacter || !OwnerCharacter->HasAuthority())
    {
        return;
    }

    if (CurrentWeapon && CurrentWeapon != UnarmedWeaponInstance)
    {
        CurrentWeapon->Destroy();
    }

    if (NewWeapon)
    {
        CurrentWeapon = NewWeapon;
    }
    else
    {
        CurrentWeapon = UnarmedWeaponInstance;
    }

    if (CurrentWeapon)
    {
        CurrentWeapon->PreloadWeaponAssets();
        CurrentWeapon->SetOwner(OwnerCharacter);
    }

    OnRep_CurrentWeapon();
}

void UCombatComponent::SetCurrentWeapon(AActor* NewWeaponActor)
{
    if (OwnerCharacter && !OwnerCharacter->HasAuthority())
    {
        return;
    }

    AWeapon_Item_Base* NewWeapon = Cast<AWeapon_Item_Base>(NewWeaponActor);
    if (!NewWeapon)
    {
        NewWeapon = UnarmedWeaponInstance;
    }

    if (CurrentWeapon == NewWeapon)
    {
        return;
    }

    CurrentWeapon = NewWeapon;

    if (CurrentWeapon)
    {
        CurrentWeapon->SetOwner(OwnerCharacter);
    }
    
    /*EWeaponType WeaponType = CurrentWeapon ? CurrentWeapon->GetWeaponType() : EWeaponType::Unarmed;
    OnWeaponChanged.Broadcast(WeaponType);*/

    OnRep_CurrentWeapon();

    // Reset combat state and stop montage when weapon changes
    ResetCombatState();

    if (OwnerCharacter)
    {
        UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
        if (AnimInstance && ActiveAttackMontage && AnimInstance->Montage_IsPlaying(ActiveAttackMontage))
        {
            AnimInstance->Montage_Stop(0.1f, ActiveAttackMontage);
        }
    }
}

void UCombatComponent::EnableWeaponCollision()
{
    if (CurrentWeapon)
    {
        CurrentWeapon->StartAttack();
    }
}

void UCombatComponent::DisableWeaponCollision()
{
    if (CurrentWeapon)
    {
        CurrentWeapon->EndAttack();
    }
}

void UCombatComponent::OpenComboWindow()
{
    bComboWindowOpen = true;
}

void UCombatComponent::CloseComboWindow()
{
    bComboWindowOpen = false;
}

void UCombatComponent::CheckComboTransition()
{
    if (!CurrentWeapon)
    {
        return;
    }

    const TArray<FComboActionInfo>& ComboList = CurrentWeapon->GetComboActions();
    if (bSaveAttackInput && ComboList.IsValidIndex(CurrentComboIndex + 1))
    {
        bSaveAttackInput = false;
        CurrentComboIndex++;

        PlayComboAttack();
    }
}

void UCombatComponent::SetGuard(bool bInGuarding)
{
    if (bIsAttacking && bInGuarding)
    {
        return;
    }

    bIsGuarding = bInGuarding;
    UpdateGuardVisuals();

    if (OwnerCharacter && !OwnerCharacter->HasAuthority())
    {
        Server_SetGuard(bInGuarding);
    }
}

bool UCombatComponent::IsPlayingAttackMontage() const
{
    if (OwnerCharacter && ActiveAttackMontage)
    {
        UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
        if (AnimInstance)
        {
            return AnimInstance->Montage_IsPlaying(ActiveAttackMontage);
        }
    }

    return false;
}

void UCombatComponent::ResetCombatState()
{
    bIsAttacking = false;
    bSaveAttackInput = false;
    bComboWindowOpen = false;
    CurrentComboIndex = 0;
}

void UCombatComponent::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    if (ActiveAttackMontage && Montage == ActiveAttackMontage)
    {
        UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
        if (AnimInstance && AnimInstance->Montage_IsPlaying(Montage))
        {
            return;
        }

        ResetCombatState();
        ActiveAttackMontage = nullptr;
    }
}

void UCombatComponent::OnRep_bIsGuarding()
{
    UpdateGuardVisuals();
}

void UCombatComponent::OnRep_CurrentWeapon()
{
    if (!OwnerCharacter)
    {
        OwnerCharacter = Cast<ANecPlayerCharacter>(GetOwner());
    }

    EWeaponType WeaponType = IsValid(CurrentWeapon) ? CurrentWeapon->GetWeaponType() : EWeaponType::Unarmed;
    OnWeaponChanged.Broadcast(WeaponType);

    if (IsValid(CurrentWeapon) && IsValid(OwnerCharacter) && IsValid(OwnerCharacter->GetMesh()))
    {
        CurrentWeapon->AttachToComponent(
            OwnerCharacter->GetMesh(),
            FAttachmentTransformRules::SnapToTargetIncludingScale,
            FName("hand_r_weapon")
        );
    }

    /*CurrentWeapon->AttachToComponent(
        OwnerCharacter->GetMesh(),
        FAttachmentTransformRules::SnapToTargetIncludingScale,
        FName("hand_r_weapon")
    );*/
}

void UCombatComponent::UpdateGuardVisuals()
{
    if (!OwnerCharacter || !CurrentWeapon)
    {
        return;
    }

    UAnimMontage* GuardMontage = CurrentWeapon->GetGuardMontage();
    if (!GuardMontage)
    {
        return;
    }

    UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
    if (!AnimInstance)
    {
        return;
    }

    if (bIsGuarding)
    {
        if (!AnimInstance->Montage_IsPlaying(GuardMontage))
        {
            OwnerCharacter->PlayAnimMontage(GuardMontage, 1.0f, TEXT("StartGuard"));
        }
    }
    else
    {
        if (AnimInstance->Montage_IsPlaying(GuardMontage))
        {
            AnimInstance->Montage_JumpToSection(TEXT("EndGuard"), GuardMontage);
        }
    }
}

void UCombatComponent::Server_ApplyDamage_Implementation(AActor* HitActor, FVector HitLocation, AWeapon_Item_Base* Weapon)
{
    if (!HitActor || !Weapon || !OwnerCharacter)
    {
        return;
    }

    float FinalDamage = Weapon->WeaponData ? Weapon->WeaponData->BaseDamage * Weapon->CurrentDamageMultiplier : 0.0f;

    UGameplayStatics::ApplyDamage(
        HitActor,
        FinalDamage,
        OwnerCharacter->GetController(),
        Weapon,
        UNecDamageType::StaticClass()
    );
    
    Weapon->Multicast_PlayHitSound(HitLocation);
}

bool UCombatComponent::Server_ApplyDamage_Validate(AActor* HitActor, FVector HitLocation, AWeapon_Item_Base* Weapon)
{
    return true;
}

void UCombatComponent::PlayComboAttack()
{
    const TArray<FComboActionInfo>& ComboList = CurrentWeapon->GetComboActions();
    if (!ComboList.IsValidIndex(CurrentComboIndex))
    {
        return;
    }

    float StaminaCost = ComboList[CurrentComboIndex].StaminaCost;
    UStaminaComponent* StaminaComp = OwnerCharacter->GetStaminaComponent();

    if (StaminaComp && StaminaComp->GetCurrentStamina() >= StaminaCost)
    {
        StaminaComp->ConsumeStamina_Predictive(StaminaCost);

        float DamageMultiplier = ComboList[CurrentComboIndex].DamageMultiplier;
        CurrentWeapon->SetDamageMultiplier(DamageMultiplier);

        float PoiseDamageMultplier = ComboList[CurrentComboIndex].PoiseDamageMultiplier;
        CurrentWeapon->SetPoiseDamageMultiplier(PoiseDamageMultplier);

        UAnimMontage* AttackMontage = CurrentWeapon->GetAttackMontage();
        FName MontageSectionName = ComboList[CurrentComboIndex].MontageSectionName;

        ActiveAttackMontage = AttackMontage;

        UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
        if (AttackMontage && AnimInstance)
        {
            if (!AnimInstance->OnMontageEnded.IsAlreadyBound(this, &UCombatComponent::OnAttackMontageEnded))
            {
                AnimInstance->OnMontageEnded.AddDynamic(this, &UCombatComponent::OnAttackMontageEnded);
            }

            if (AnimInstance->Montage_IsPlaying(AttackMontage))
            {
                AnimInstance->Montage_JumpToSection(MontageSectionName, AttackMontage);
            }
            else
            {
                OwnerCharacter->PlayAnimMontage(AttackMontage, 1.0f, MontageSectionName);
            }
        }

        Server_Attack(CurrentComboIndex);
    }
    else
    {
        ResetCombatState();
    }
}

void UCombatComponent::PlayRunningAttack()
{
    if (!CurrentWeapon)
    {
        return;
    }

    UAnimMontage* RunningMontage = CurrentWeapon->GetRunningAttackMontage();
    if (!RunningMontage)
    {
        return;
    }

    float StaminaCost = CurrentWeapon->WeaponData->RunningAttackStaminaCost;
    UStaminaComponent* StaminaComp = OwnerCharacter->GetStaminaComponent();

    if (StaminaComp && StaminaComp->GetCurrentStamina() >= StaminaCost)
    {
        StaminaComp->ConsumeStamina_Predictive(StaminaCost);

        float RunningAttackDamageMultiplier = CurrentWeapon->WeaponData->RunningAttackDamageMultiplier;
        CurrentWeapon->SetDamageMultiplier(RunningAttackDamageMultiplier);
        CurrentWeapon->SetPoiseDamageMultiplier(RunningAttackDamageMultiplier);

        ActiveAttackMontage = RunningMontage;

        UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
        if (AnimInstance)
        {
            if (!AnimInstance->OnMontageEnded.IsAlreadyBound(this, &UCombatComponent::OnAttackMontageEnded))
            {
                AnimInstance->OnMontageEnded.AddDynamic(this, &UCombatComponent::OnAttackMontageEnded);
            }

            OwnerCharacter->PlayAnimMontage(RunningMontage, 1.0f);
        }

        Server_RunningAttack();
    }
    else
    {
        ResetCombatState();
    }
}

void UCombatComponent::Multicast_RunningAttack_Implementation()
{
    if (!OwnerCharacter || !CurrentWeapon)
    {
        return;
    }

    if (OwnerCharacter->IsLocallyControlled())
    {
        return;
    }

    UAnimMontage* RunningMontage = CurrentWeapon->GetRunningAttackMontage();
    UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();

    if (RunningMontage && AnimInstance)
    {
        OwnerCharacter->PlayAnimMontage(RunningMontage, 1.0f);
    }
}

void UCombatComponent::Server_RunningAttack_Implementation()
{
    if (!OwnerCharacter || !CurrentWeapon)
    {
        return;
    }
    
    float StaminaCost = CurrentWeapon->WeaponData->RunningAttackStaminaCost;
    UStaminaComponent* StaminaComp = OwnerCharacter->GetStaminaComponent();

    if (StaminaComp && StaminaComp->GetCurrentStamina() >= StaminaCost)
    {
        StaminaComp->ConsumeStamina(StaminaCost);

        float RunningAttackDamageMultiplier = CurrentWeapon->WeaponData->RunningAttackDamageMultiplier;
        CurrentWeapon->SetDamageMultiplier(RunningAttackDamageMultiplier);
        CurrentWeapon->SetPoiseDamageMultiplier(RunningAttackDamageMultiplier);

        Multicast_RunningAttack();
    }
}

void UCombatComponent::Multicast_Attack_Implementation(int32 ComboIndex)
{
    if (!OwnerCharacter || !CurrentWeapon)
    {
        return;
    }

    if (OwnerCharacter->IsLocallyControlled())
    {
        return;
    }

    const TArray<FComboActionInfo>& ComboList = CurrentWeapon->GetComboActions();
    if (!ComboList.IsValidIndex(ComboIndex))
    {
        return;
    }

    UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
    UAnimMontage* AttackMontage = CurrentWeapon->GetAttackMontage();
    FName MontageSectionName = ComboList[ComboIndex].MontageSectionName;

    if (AttackMontage && AnimInstance)
    {
        if (AnimInstance->Montage_IsPlaying(AttackMontage))
        {
            AnimInstance->Montage_JumpToSection(MontageSectionName, AttackMontage);
        }
        else
        {
            OwnerCharacter->PlayAnimMontage(AttackMontage, 1.0f, MontageSectionName);
        }
    }
}

void UCombatComponent::Server_SetGuard_Implementation(bool bInGuarding)
{
    bIsGuarding = bInGuarding;

    UpdateGuardVisuals();
}

void UCombatComponent::Server_Attack_Implementation(int32 ComboIndex)
{
    if (!OwnerCharacter || !CurrentWeapon)
    {
        return;
    }

    const TArray<FComboActionInfo>& ComboList = CurrentWeapon->GetComboActions();
    if (!ComboList.IsValidIndex(ComboIndex))
    {
        return;
    }

    float StaminaCost = ComboList[ComboIndex].StaminaCost;
    UStaminaComponent* StaminaComp = OwnerCharacter->GetStaminaComponent();

    if (StaminaComp && StaminaComp->GetCurrentStamina() >= StaminaCost)
    {
        StaminaComp->ConsumeStamina(StaminaCost);

        float DamageMultiplier = ComboList[ComboIndex].DamageMultiplier;
        CurrentWeapon->SetDamageMultiplier(DamageMultiplier);

        float PoiseDamageMultplier = ComboList[ComboIndex].PoiseDamageMultiplier;
        CurrentWeapon->SetPoiseDamageMultiplier(PoiseDamageMultplier);

        Multicast_Attack(ComboIndex);
    }
}