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
#include "GridInventory/ItemData/ItemDataSubsystem.h"


AMonsterBase::AMonsterBase()
{
	MonsterStatComponent = CreateDefaultSubobject<UMonsterStatComponent>(TEXT("MonsterStatComponent"));
	SetRVOAvoidanceEnabled(true);

	bReplicates = true;
	SetReplicatingMovement(true);
	SetNetUpdateFrequency(10.0f);
	SetMinNetUpdateFrequency(2.0f);

	
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

void AMonsterBase::ForceCleanupAttackState()
{
	if (AAIController* AIC = GetController<AAIController>())
	{
		if (UBlackboardComponent* BB = AIC->GetBlackboardComponent())
		{
			BB->SetValueAsBool(NAME_IsAttacking, false);
		}
	}

	OnNextComboRequested.Unbind();
	RestoreMovementIfAlive();

	if (UWorld* World = GetWorld())
	{
		if (UMonsterEngagementSubsystem* Engagement = World->GetSubsystem<UMonsterEngagementSubsystem>())
		{
			Engagement->ReleaseAllSlotsForMonster(this);
		}
	}
}

void AMonsterBase::RestoreMovementIfAlive()
{
	if (!bIsDead)
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
}

void AMonsterBase::SetCombatMovementMode(bool bCombat)
{
	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	if (!MoveComp)
	{
		return;
	}

	if (bCombat)
	{
		// 전투: 이동 방향 회전 끄고, SmoothLookAt이 직접 회전 관리
		MoveComp->bOrientRotationToMovement = false;
	}
	else
	{
		// 순찰: 이동 방향으로 자연스럽게 몸 회전
		MoveComp->bOrientRotationToMovement = true;
	}
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
		MoveComp->bOrientRotationToMovement = true;
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
			BB->SetValueAsBool(NAME_IsStaggered, true);
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
			BB->SetValueAsBool(NAME_IsStaggered, true);
		}
	}
}

void AMonsterBase::OnDeath()
{
	if (!HasAuthority()) return;

	bIsDead = true;

	ForceCleanupAttackState();

	AAIController* AIController = GetController<AAIController>();
	if (AIController && AIController->GetBrainComponent())
	{
		AIController->GetBrainComponent()->StopLogic("Dead");
		AIController->UnPossess();
	}

	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();

	SpawnDropItems();

	Multicast_PlayDeathMontage();
}

void AMonsterBase::SpawnDropItems()
{
	if (DropItemIDs.Num() == 0)
	{
		return;
	}

	UGameInstance* GI = GetGameInstance();
	if (!GI)
	{
		return;
	}

	UDataTableSubsystem* ItemSubsystem = GI->GetSubsystem<UDataTableSubsystem>();
	if (!ItemSubsystem)
	{
		return;
	}

	FVector DeathLocation = GetActorLocation();

	for (const FName& ItemID : DropItemIDs)
	{
		const FItemData* Data = ItemSubsystem->GetItemData(ItemID);
		if (!Data || !Data->DropItemActorClass)
		{
			UE_LOG(LogTemp, Warning, TEXT("[MonsterBase] DropItem failed: ItemID=%s - Data or DropItemActorClass is null"), *ItemID.ToString());
			continue;
		}

		FVector Offset = FVector(
			FMath::FRandRange(-DropSpreadRadius, DropSpreadRadius),
			FMath::FRandRange(-DropSpreadRadius, DropSpreadRadius),
			0.0f
		);
		FVector SpawnLocation = DeathLocation + Offset;

		GetWorld()->SpawnActor<AActor>(
			Data->DropItemActorClass,
			SpawnLocation,
			FRotator::ZeroRotator
		);
	}
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
		float RagdollDelay = FMath::Max(0.1f, Duration - 0.5f);
		GetWorldTimerManager().SetTimer(DeathTimerHandle, this, &AMonsterBase::StartRagdoll, RagdollDelay, false);
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
			BB->SetValueAsBool(NAME_IsAttacking, false);
		}
	}

	// 피격 시 공격 슬롯이 없으면 임시 부여
	TryGrantTemporarySlot();

	MonsterStatComponent->ApplyPoise(DamageAmount);

	if (HitReactMontage)
	{
		GetCharacterMovement()->StopMovementImmediately();
		GetCharacterMovement()->DisableMovement();

		if (HasAuthority())
		{
			Multicast_PlayMontage(HitReactMontage);

			if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
			{
				FOnMontageEnded EndDelegate;
				EndDelegate.BindUObject(this, &AMonsterBase::OnHitReactMontageEnded);
				AnimInstance->Montage_SetEndDelegate(EndDelegate, HitReactMontage);
			}
			else
			{
				RestoreMovementIfAlive();
			}
		}
	}
}

void AMonsterBase::TryGrantTemporarySlot()
{
	if (!HasAuthority())
	{
		return;
	}

	AAIController* AIC = GetController<AAIController>();
	if (!AIC)
	{
		return;
	}

	UBlackboardComponent* BB = AIC->GetBlackboardComponent();
	if (!BB)
	{
		return;
	}

	AActor* Target = Cast<AActor>(BB->GetValueAsObject(NAME_TargetActor));
	if (!Target)
	{
		return;
	}

	UMonsterEngagementSubsystem* Engagement = GetWorld()->GetSubsystem<UMonsterEngagementSubsystem>();
	if (!Engagement)
	{
		return;
	}

	if (!Engagement->HasAttackSlot(this, Target))
	{
		Engagement->GrantTemporarySlot(this, Target);
	}
}

void AMonsterBase::OnHitReactMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!bIsDead)
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}

	if (AAIController* AIC = GetController<AAIController>())
	{
		if (UBlackboardComponent* BB = AIC->GetBlackboardComponent())
		{
			BB->SetValueAsBool(NAME_IsStaggered, false);
		}
	}
}