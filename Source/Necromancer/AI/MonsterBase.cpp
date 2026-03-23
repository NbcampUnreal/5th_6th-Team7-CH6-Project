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
	MotionWarpingComp = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarping"));
	SetRVOAvoidanceEnabled(true);

	bReplicates = true;
	SetReplicatingMovement(true);
	SetNetUpdateFrequency(10.0f);
	SetMinNetUpdateFrequency(2.0f);


	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	// 대기 사운드 AudioComponent 생성
	IdleAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("IdleAudioComp"));
	IdleAudioComp->SetupAttachment(RootComponent);
	IdleAudioComp->bAutoActivate = false;
	IdleAudioComp->bOverrideAttenuation = true;
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

float AMonsterBase::GetPoiseDamage() const
{
	return MonsterStatComponent->GetPoiseDamage();
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
		// 전투: 이동 방향 회전 끄고, Controller 회전(SetFocus)으로 타겟을 바라봄
		MoveComp->bOrientRotationToMovement = false;
		MoveComp->bUseControllerDesiredRotation = true;
		MoveComp->RotationRate = FRotator(0.0f, CombatRotationSpeed, 0.0f);
	}
	else
	{
		// 순찰: 이동 방향으로 자연스럽게 몸 회전
		MoveComp->bOrientRotationToMovement = true;
		MoveComp->bUseControllerDesiredRotation = false;
	}
}

void AMonsterBase::BeginPlay()
{
	Super::BeginPlay();

	// 몬스터 메시에 데칼 투영 차단 (AOE 인디케이터가 메시에 묻는 것 방지)
	GetMesh()->SetReceivesDecals(false);

	MonsterStatComponent->OnDamageReceived.AddDynamic(this, &AMonsterBase::OnDamageReceived);

	MonsterStatComponent->OnDeath.AddDynamic(this, &AMonsterBase::OnDeath);
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->bOrientRotationToMovement = true;
		MoveComp->bUseControllerDesiredRotation = false;


		MoveComp->MaxAcceleration = MovementAcceleration;
		MoveComp->BrakingDecelerationWalking = MovementDeceleration;
	}

	// 대기 사운드 설정 + 재생 시작
	if (IdleAudioComp && IdleSound)
	{
		IdleAudioComp->SetSound(IdleSound);
		IdleAudioComp->SetVolumeMultiplier(IdleSoundVolume);
		IdleAudioComp->Play();
	}
}

void AMonsterBase::OnDeath()
{
	if (!HasAuthority()) return;

	bIsDead = true;

	// 대기 사운드 영구 중지
	SetIdleSoundActive(false);

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
	DOREPLIFETIME(AMonsterBase, bIsBlocking);
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

void AMonsterBase::OnDamageReceived(float DamageAmount, FVector HitLocation, bool bPoiseBroken)
{
	if (bIsDead)
	{
		return;
	}

	// 블로킹 중이면 블록 리액션 (포이즈는 HandleTakeDamage에서 이미 감소 적용됨)
	if (bIsBlocking)
	{
		if (bPoiseBroken)
		{
			if (AAIController* AIC = GetController<AAIController>())
			{
				if (UBlackboardComponent* BB = AIC->GetBlackboardComponent())
				{
					BB->SetValueAsBool(NAME_IsStaggered, true);
				}
			}
		}

		if (HasAuthority() && BlockReactMontage)
		{
			Multicast_PlayMontage(BlockReactMontage);
		}
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

	TryGrantTemporarySlot();

	// bPoiseBroken이면 경직 상태 설정
	if (bPoiseBroken)
	{
		if (AAIController* AIC = GetController<AAIController>())
		{
			if (UBlackboardComponent* BB = AIC->GetBlackboardComponent())
			{
				BB->SetValueAsBool(NAME_IsStaggered, true);
			}
		}
	}

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

void AMonsterBase::SetBlockingState(bool bBlock)
{
	bIsBlocking = bBlock;
}

void AMonsterBase::OnRep_IsBlocking()
{
}

void AMonsterBase::SetWarpTarget(FName WarpTargetName, FVector TargetLocation, FRotator TargetRotation)
{
	if (MotionWarpingComp)
	{
		MotionWarpingComp->AddOrUpdateWarpTargetFromLocationAndRotation(
			WarpTargetName, TargetLocation, FRotator(0.f, TargetRotation.Yaw, 0.f));
	}
}

void AMonsterBase::ClearWarpTarget(FName WarpTargetName)
{
	if (MotionWarpingComp)
	{
		MotionWarpingComp->RemoveWarpTarget(WarpTargetName);
	}
}

void AMonsterBase::SetIdleSoundActive(bool bActive)
{
	if (!IdleAudioComp || !IdleSound)
	{
		return;
	}

	if (bActive && !IdleAudioComp->IsPlaying())
	{
		IdleAudioComp->Play();
	}
	else if (!bActive && IdleAudioComp->IsPlaying())
	{
		IdleAudioComp->Stop();
	}
}