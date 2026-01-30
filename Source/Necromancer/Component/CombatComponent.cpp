#include "Component/CombatComponent.h"
#include "Character/NecPlayerCharacter.h"
#include "Component/StaminaComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

UCombatComponent::UCombatComponent() :
    AttackMontage(nullptr)
{
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ANecPlayerCharacter>(GetOwner());
}

void UCombatComponent::Attack()
{
    if (!OwnerCharacter || !OwnerCharacter->GetStaminaComponent())
    {
        return;
    }
        
    if (OwnerCharacter->GetMesh()->GetAnimInstance() && AttackMontage)
    {
        if (OwnerCharacter->GetMesh()->GetAnimInstance()->Montage_IsPlaying(AttackMontage))
        {
            return;
        }
    }

    UStaminaComponent* StaminaComp = OwnerCharacter->GetStaminaComponent();

    if (StaminaComp->IsExhausted() || StaminaComp->GetCurrentStamina() < AttackStaminaCost)
    {
        return;
    }

    if (AttackMontage)
    {
        UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
        if (AnimInstance)
        {
            if (!AnimInstance->Montage_IsPlaying(AttackMontage))
            {
                OwnerCharacter->PlayAnimMontage(AttackMontage);
            }
        }
    }

    StaminaComp->ConsumeStamina_Predictive(AttackStaminaCost);

    Server_Attack();
}

void UCombatComponent::PerformAttackTrace()
{
    if (!GetOwner()->HasAuthority())
    {
        return;
    }

    if (bHasAppliedDamageInCurrentAttack)
    {
        return;
    }

    if (!OwnerCharacter)
    {
        return;
    }

    FVector Start = OwnerCharacter->GetActorLocation();
    FVector End = Start + (OwnerCharacter->GetActorForwardVector() * AttackRange);
    TArray<AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(OwnerCharacter);

    FHitResult HitResult;

    bool bHit = UKismetSystemLibrary::SphereTraceSingleForObjects(
        GetWorld(),
        Start,
        End,
        50.0f,
        { UEngineTypes::ConvertToObjectType(ECC_Pawn) },
        false,
        ActorsToIgnore,
        EDrawDebugTrace::ForDuration,
        HitResult,
        true
    );

    if (bHit && HitResult.GetActor())
    {
        bHasAppliedDamageInCurrentAttack = true;

        UGameplayStatics::ApplyDamage(
            HitResult.GetActor(),
            AttackDamage,
            OwnerCharacter->GetController(),
            OwnerCharacter,
            UDamageType::StaticClass()
        );

        UE_LOG(LogTemp, Log, TEXT("Hit Actor: %s"), *HitResult.GetActor()->GetName());
    }
}

void UCombatComponent::Server_Attack_Implementation()
{
    if (!OwnerCharacter)
    {
        return;
    }

    UStaminaComponent* StaminaComp = OwnerCharacter->GetStaminaComponent();
    if (!StaminaComp)
    {
        return;
    }

    if (StaminaComp->GetCurrentStamina() >= AttackStaminaCost)
    {
        StaminaComp->ConsumeStamina(AttackStaminaCost);

        bHasAppliedDamageInCurrentAttack = false;

        if (AttackMontage)
        {
            OwnerCharacter->PlayAnimMontage(AttackMontage);
        }
    }
}