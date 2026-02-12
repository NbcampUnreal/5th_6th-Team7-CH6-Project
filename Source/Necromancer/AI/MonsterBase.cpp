// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterBase.h"

#include "AIController.h"
#include "BrainComponent.h"
#include "MonsterEngagementSubsystem.h"
#include "MonsterStatComponent.h"
#include "Necromancer.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Net/UnrealNetwork.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


AMonsterBase::AMonsterBase()
{
	MonsterStatComponent = CreateDefaultSubobject<UMonsterStatComponent>(TEXT("MonsterStatComponent"));
	SetRVOAvoidanceEnabled(true);

	bReplicates = true;
	SetReplicatingMovement(true);
	NetUpdateFrequency = 10.0f;
	MinNetUpdateFrequency = 2.0f;

	
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
}


void AMonsterBase::SetRVOAvoidanceEnabled(bool bEnable)
{
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	if (MovementComponent)
	{
		MovementComponent ->bUseRVOAvoidance = bEnable;
		MovementComponent -> AvoidanceConsiderationRadius = AvoidanceRadius;
		MovementComponent -> AvoidanceWeight = AvoidanceWeight;
	}
}

bool AMonsterBase::GetIsDead()
{
	return bIsDead;
}

void AMonsterBase::BeginPlay()
{
	Super::BeginPlay();

	MonsterStatComponent->OnDamageReceived.AddDynamic(this, &AMonsterBase::OnDamageReceived);

	MonsterStatComponent->OnDeath.AddDynamic(this, &AMonsterBase::OnDeath);
	MonsterStatComponent->OnStagger.AddUObject(this, &AMonsterBase::OnStagger);
	MonsterStatComponent->OnStun.AddUObject(this, &AMonsterBase::OnStun);

	
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->bOrientRotationToMovement = false;
		MoveComp->bUseControllerDesiredRotation = false;

		
		MoveComp->MaxAcceleration = MovementAcceleration;
		MoveComp->BrakingDecelerationWalking = MovementDeceleration;
	}
}

void AMonsterBase::OnStagger()
{
	StopAnimMontage();

	if (AAIController* AIC = GetController<AAIController>())
	{
		if (UBlackboardComponent* BB = AIC->GetBlackboardComponent())
		{
			BB->SetValueAsBool(FName(NAME_IsStaggered), true);
		}
	}
}

void AMonsterBase::OnStun()
{
	StopAnimMontage();

	if (AAIController* AIC = GetController<AAIController>())
	{
		if (UBlackboardComponent* BB = AIC->GetBlackboardComponent())
		{
			BB->SetValueAsBool(FName(NAME_IsStaggered), true);
		}
	}
}

void AMonsterBase::OnDeath()
{
	if (!HasAuthority()) return;

	bIsDead = true;

	
	if (UMonsterEngagementSubsystem* Engagement = GetWorld()->GetSubsystem<UMonsterEngagementSubsystem>())
	{
		Engagement->ReleaseAllSlotsForMonster(this);
	}

	AAIController* AIController = GetController<AAIController>();
	if (AIController && AIController->GetBrainComponent())
	{
		AIController->GetBrainComponent()->StopLogic("Dead");
		AIController->UnPossess();
	}

	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();

	Multicast_PlayDeathMontage();
}

void AMonsterBase::StartRagdoll()
{
	GetMesh()->SetAllBodiesSimulatePhysics(true);
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

FGenericTeamId AMonsterBase::GetGenericTeamId() const
{
	return FGenericTeamId(TEAM_ID_MONSTER);
}

void AMonsterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMonsterBase, bIsDead);
}

void AMonsterBase::OnRep_IsDead()
{
	if (bIsDead)
	{
		GetCharacterMovement()->DisableMovement();
	}
}

void AMonsterBase::Multicast_PlayDeathMontage_Implementation()
{
	if (DeathMontage)
	{
		float Duration = PlayAnimMontage(DeathMontage);
		GetWorldTimerManager().SetTimer(DeathTimerHandle, this, &AMonsterBase::StartRagdoll, Duration-0.5, false);
	}
	else
	{
		StartRagdoll();
	}
}

void AMonsterBase::Multicast_PlayMontage_Implementation(UAnimMontage* Montage)
{
	if (Montage)
	{
		PlayAnimMontage(Montage);
	}
}

void AMonsterBase::OnDamageReceived(float DamageAmount, FVector HitLocation)
{
	if (bIsDead)
	{
		return;
	}

	
	StopAnimMontage();

	
	if (AAIController* AIC = GetController<AAIController>())
	{
		if (UBlackboardComponent* BB = AIC->GetBlackboardComponent())
		{
			BB->SetValueAsBool(FName(NAME_IsAttacking), false);
		}
	}

	
	MonsterStatComponent->ApplyPoise(DamageAmount);

	
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();

	
	if (HitReactMontage)
	{
		Multicast_PlayMontage(HitReactMontage);

		
		if (HasAuthority())
		{
			if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
			{
				FOnMontageEnded EndDelegate;
				EndDelegate.BindUObject(this, &AMonsterBase::OnHitReactMontageEnded);
				AnimInstance->Montage_SetEndDelegate(EndDelegate, HitReactMontage);
			}
		}
	}
}

void AMonsterBase::OnHitReactMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	// 이동 복구
	if (!bIsDead)
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}

	if (AAIController* AIC = GetController<AAIController>())
	{
		if (UBlackboardComponent* BB = AIC->GetBlackboardComponent())
		{
			BB->SetValueAsBool(FName(NAME_IsStaggered), false);
		}
	}
}