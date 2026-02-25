#include "Component/CombatComponent.h"
#include "Character/NecPlayerCharacter.h"
#include "Component/StaminaComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Item/Weapon_Item_Base.h"

UCombatComponent::UCombatComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

    SetIsReplicatedByDefault(true);
}

void UCombatComponent::BeginPlay()
{
    Super::BeginPlay();

    OwnerCharacter = Cast<ANecPlayerCharacter>(GetOwner());
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UCombatComponent, bIsGuarding);
    DOREPLIFETIME(UCombatComponent, CurrentWeapon);
}

void UCombatComponent::Attack()
{
    if (!OwnerCharacter || !CurrentWeapon)
    {
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

    if (CurrentWeapon)
    {
        CurrentWeapon->Destroy();
    }

    CurrentWeapon = NewWeapon;

    if (CurrentWeapon)
    {
        CurrentWeapon->SetOwner(OwnerCharacter);

        CurrentWeapon->AttachToComponent(
            OwnerCharacter->GetMesh(),
            FAttachmentTransformRules::SnapToTargetIncludingScale,
            FName("hand_r_weapon")
        );
    }
}

void UCombatComponent::SetCurrentWeapon(AActor* NewWeaponActor)
{
    AWeapon_Item_Base* NewWeapon = Cast<AWeapon_Item_Base>(NewWeaponActor);

    if (CurrentWeapon == NewWeapon)
    {
        return;
    }

    CurrentWeapon = NewWeapon;
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
    if (OwnerCharacter && OwnerCharacter->HasAuthority())
    {
        bIsGuarding = bInGuarding;
        UpdateGuardVisuals();
    }
    else
    {
        Server_SetGuard(bInGuarding);
    }
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
    if (CurrentWeapon && Montage == CurrentWeapon->GetAttackMontage())
    {
        UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
        if (AnimInstance && AnimInstance->Montage_IsPlaying(Montage))
        {
            return;
        }

        ResetCombatState();
    }
}

void UCombatComponent::OnRep_bIsGuarding()
{
    UpdateGuardVisuals();
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

        float Multiplier = ComboList[CurrentComboIndex].DamageMultiplier;
        CurrentWeapon->SetDamageMultiplier(Multiplier);

        UAnimMontage* AttackMontage = CurrentWeapon->GetAttackMontage();
        FName MontageSectionName = ComboList[CurrentComboIndex].MontageSectionName;

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

        Multicast_Attack(ComboIndex);
    }
}