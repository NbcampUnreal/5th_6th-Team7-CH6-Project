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

    UAnimMontage* AttackMontage = CurrentWeapon->GetAttackMontage();
    if (AttackMontage)
    {
        UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
        if (AnimInstance && AnimInstance->Montage_IsPlaying(AttackMontage))
        {
            return;
        }

        UStaminaComponent* StaminaComp = OwnerCharacter->GetStaminaComponent();
        float StaminaCost = CurrentWeapon->GetStaminaCost();

        if (StaminaComp && StaminaComp->GetCurrentStamina() < StaminaCost)
        {
            return;
        }

        OwnerCharacter->PlayAnimMontage(AttackMontage);
        StaminaComp->ConsumeStamina_Predictive(StaminaCost);

        Server_Attack();
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
            FName("hand_r")
        );
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

void UCombatComponent::SetGuard(bool bInGuarding)
{
    if (bIsGuarding == bInGuarding)
    {
        return;
    }

    bIsGuarding = bInGuarding;
    UpdateGuardVisuals();

    Server_SetGuard(bInGuarding);
}

void UCombatComponent::OnRep_bIsGuarding()
{
    UpdateGuardVisuals();
}

void UCombatComponent::UpdateGuardVisuals()
{
    if (!OwnerCharacter || !GuardMontage)
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
        OwnerCharacter->PlayAnimMontage(GuardMontage, 1.0f, TEXT("StartGuard"));
    }
    else
    {
        if (AnimInstance->Montage_IsPlaying(GuardMontage))
        {
            AnimInstance->Montage_JumpToSection(TEXT("EndGuard"), GuardMontage);
        }
    }
}

void UCombatComponent::Multicast_Attack_Implementation()
{
    if (!OwnerCharacter || !CurrentWeapon)
    {
        return;
    }

    if (OwnerCharacter->IsLocallyControlled())
    {
        return;
    }

    UAnimMontage* AttackMontage = CurrentWeapon->GetAttackMontage();
    if (AttackMontage)
    {
        OwnerCharacter->PlayAnimMontage(AttackMontage);
    }
}

void UCombatComponent::Server_SetGuard_Implementation(bool bInGuarding)
{    
    bIsGuarding = bInGuarding;

    if (GetOwnerRole() == ROLE_Authority)
    {
        UpdateGuardVisuals();
    }
}

void UCombatComponent::Server_Attack_Implementation()
{
    if (!OwnerCharacter || !CurrentWeapon)
    {
        return;
    }

    UAnimMontage* AttackMontage = CurrentWeapon->GetAttackMontage();
    UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();

    if (!OwnerCharacter->IsLocallyControlled() && AttackMontage && AnimInstance && AnimInstance->Montage_IsPlaying(AttackMontage))
    {
        return;
    }
    
    UStaminaComponent* StaminaComp = OwnerCharacter->GetStaminaComponent();
    float StaminaCost = CurrentWeapon->GetStaminaCost();

    if (StaminaComp && StaminaComp->ConsumeStamina(StaminaCost))
    {
        Multicast_Attack();
    }
}