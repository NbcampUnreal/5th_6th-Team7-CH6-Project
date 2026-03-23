#include "BTTask_BossAction.h"
#include "AIController.h"
#include "BossMonsterBase.h"
#include "MonsterEngagementSubsystem.h"
#include "MonsterStatComponent.h"
#include "Necromancer.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NavigationSystem.h"
#include "TimerManager.h"

UBTTask_BossAction::UBTTask_BossAction()
{
	NodeName = "Boss Action";
	bCreateNodeInstance = true;
	bNotifyTick = false;
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UBTTask_BossAction::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (!ActionSet)
	{
		return EBTNodeResult::Failed;
	}

	AAIController* AIC = OwnerComp.GetAIOwner();
	if (!AIC)
	{
		return EBTNodeResult::Failed;
	}

	ABossMonsterBase* Boss = Cast<ABossMonsterBase>(AIC->GetPawn());
	if (!Boss)
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB)
	{
		return EBTNodeResult::Failed;
	}

	UMonsterStatComponent* StatComp = Boss->FindComponentByClass<UMonsterStatComponent>();
	if (StatComp && !StatComp->CanAttack())
	{
		return EBTNodeResult::Failed;
	}

	// 현재 페이즈 + 타겟 거리
	int32 Phase = BB->GetValueAsInt(NAME_BossPhase);
	if (Phase < 1) Phase = 1;

	float DistanceToTarget = 0.0f;
	AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(NAME_TargetActor));
	if (TargetActor)
	{
		DistanceToTarget = FVector::Dist(Boss->GetActorLocation(), TargetActor->GetActorLocation());
	}

	// 현재 월드 시간
	float CurrentTime = Boss->GetWorld()->GetTimeSeconds();

	// 액션 선택
	int32 SelectedIndex = INDEX_NONE;
	const FBossActionEntry* SelectedAction = ActionSet->SelectAction(Phase, DistanceToTarget, ActionCooldowns, CurrentTime, SelectedIndex);

	if (!SelectedAction)
	{
		return EBTNodeResult::Failed;
	}

	// 쿨타임 기록
	if (SelectedAction->Cooldown > 0.0f && SelectedIndex != INDEX_NONE)
	{
		ActionCooldowns.Add(SelectedIndex, CurrentTime);
	}

	// 데미지 배율 적용
	if (StatComp)
	{
		StatComp->SetDamageMultiplier(SelectedAction->DamageMultiplier);
	}

	// 슈퍼아머 설정 (이 패턴 실행 중 HitReact 무시)
	if (SelectedAction->bSuperArmor)
	{
		Boss->SetSuperArmor(true);
	}

	// 공격 중 대기 사운드 중지
	Boss->SetIdleSoundActive(false);

	// 타입별 실행
	switch (SelectedAction->ActionType)
	{
	case EBossActionType::Melee:
		ExecuteMeleeAction(Boss, *SelectedAction, OwnerComp);
		break;

	case EBossActionType::AOE:
		ExecuteAOEAction(Boss, *SelectedAction, OwnerComp);
		break;

	case EBossActionType::Teleport:
		ExecuteTeleportAction(Boss, *SelectedAction, OwnerComp);
		break;

	default:
		return EBTNodeResult::Failed;
	}

	return EBTNodeResult::InProgress;
}

void UBTTask_BossAction::ExecuteMeleeAction(ABossMonsterBase* Boss, const FBossActionEntry& Action, UBehaviorTreeComponent& OwnerComp)
{
	AAIController* AIC = OwnerComp.GetAIOwner();
	AIC->StopMovement();
	Boss->GetCharacterMovement()->StopMovementImmediately();

	// Melee 워프 타겟 설정 (타겟 위치로 루트모션 보정)
	if (UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent())
	{
		AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(NAME_TargetActor));
		if (TargetActor)
		{
			FVector TargetLoc = TargetActor->GetActorLocation();
			FRotator LookAtRot = (TargetLoc - Boss->GetActorLocation()).Rotation();
			Boss->SetWarpTarget(FName("AttackTarget"), TargetLoc, LookAtRot);
		}
	}

	PlayActionMontage(Boss, Action.ActionMontage, OwnerComp);
}

void UBTTask_BossAction::ExecuteAOEAction(ABossMonsterBase* Boss, const FBossActionEntry& Action, UBehaviorTreeComponent& OwnerComp)
{
	AAIController* AIC = OwnerComp.GetAIOwner();
	AIC->StopMovement();
	Boss->GetCharacterMovement()->StopMovementImmediately();

	PlayActionMontage(Boss, Action.ActionMontage, OwnerComp);
}

void UBTTask_BossAction::ExecuteTeleportAction(ABossMonsterBase* Boss, const FBossActionEntry& Action, UBehaviorTreeComponent& OwnerComp)
{
	AAIController* AIC = OwnerComp.GetAIOwner();
	AIC->StopMovement();
	Boss->GetCharacterMovement()->StopMovementImmediately();

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	AActor* TargetActor = BB ? Cast<AActor>(BB->GetValueAsObject(NAME_TargetActor)) : nullptr;

	if (TargetActor && Boss->HasAuthority())
	{
		FVector TeleportLocation = CalculateTeleportLocation(TargetActor, Action);

		// 타겟을 바라보는 회전
		FVector DirectionToTarget = (TargetActor->GetActorLocation() - TeleportLocation).GetSafeNormal();
		FRotator TeleportRotation = DirectionToTarget.Rotation();
		TeleportRotation.Pitch = 0.0f;

		Boss->Multicast_Teleport(TeleportLocation, TeleportRotation);
	}

	// 순간이동 후 몽타주가 있으면 워프 타겟 설정 후 재생
	if (Action.ActionMontage)
	{
		if (TargetActor)
		{
			FVector TargetLoc = TargetActor->GetActorLocation();
			FRotator LookAtRot = (TargetLoc - Boss->GetActorLocation()).Rotation();
			Boss->SetWarpTarget(FName("AttackTarget"), TargetLoc, LookAtRot);
		}
		PlayActionMontage(Boss, Action.ActionMontage, OwnerComp);
	}
	else
	{
		// 몽타주 없으면 즉시 완료
		UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
		if (BBComp)
		{
			BBComp->SetValueAsBool(NAME_IsAttacking, true);
		}
		bTaskActive = true;

		// 짧은 딜레이 후 완료 (순간이동 연출 시간)
		if (UWorld* World = Boss->GetWorld())
		{
			World->GetTimerManager().SetTimer(
				SafetyTimerHandle,
				FTimerDelegate::CreateUObject(this, &UBTTask_BossAction::OnSafetyTimeout, &OwnerComp),
				0.5f,
				false
			);
		}
	}
}

FVector UBTTask_BossAction::CalculateTeleportLocation(AActor* Target, const FBossActionEntry& Action) const
{
	FVector TargetLocation = Target->GetActorLocation();
	FVector Result;

	switch (Action.TeleportMode)
	{
	case ETeleportMode::BehindTarget:
	{
		FVector TargetForward = Target->GetActorForwardVector();
		Result = TargetLocation + (-TargetForward * Action.TeleportDistance);
		break;
	}

	case ETeleportMode::RandomAround:
	{
		float RandomAngle = FMath::FRandRange(0.0f, 360.0f);
		FVector Offset = FVector(
			FMath::Cos(FMath::DegreesToRadians(RandomAngle)) * Action.TeleportDistance,
			FMath::Sin(FMath::DegreesToRadians(RandomAngle)) * Action.TeleportDistance,
			0.0f
		);
		Result = TargetLocation + Offset;
		break;
	}

	default:
		Result = TargetLocation;
		break;
	}

	// NavMesh 위 유효한 위치로 보정
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(Target->GetWorld());
	if (NavSys)
	{
		FNavLocation NavLocation;
		if (NavSys->ProjectPointToNavigation(Result, NavLocation, FVector(200.0f, 200.0f, 200.0f)))
		{
			Result = NavLocation.Location;
		}
	}

	return Result;
}

void UBTTask_BossAction::PlayActionMontage(ABossMonsterBase* Boss, UAnimMontage* Montage, UBehaviorTreeComponent& OwnerComp)
{
	if (!Montage)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	USkeletalMeshComponent* Mesh = Boss->GetMesh();
	if (!Mesh)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	UAnimInstance* AnimInstance = Mesh->GetAnimInstance();
	if (!AnimInstance)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	Boss->Multicast_PlayMontage(Montage);

	float Duration = Montage->GetPlayLength();
	if (Duration <= 0.0f)
	{
		Boss->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (BB)
	{
		BB->SetValueAsBool(NAME_IsAttacking, true);
	}
	bTaskActive = true;

	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &UBTTask_BossAction::OnMontageEnded, &OwnerComp);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, Montage);

	if (UWorld* World = Boss->GetWorld())
	{
		World->GetTimerManager().SetTimer(
			SafetyTimerHandle,
			FTimerDelegate::CreateUObject(this, &UBTTask_BossAction::OnSafetyTimeout, &OwnerComp),
			Duration + TimeoutBuffer,
			false
		);
	}
}

void UBTTask_BossAction::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted, UBehaviorTreeComponent* OwnerComp)
{
	if (!OwnerComp || !bTaskActive)
	{
		return;
	}

	CleanupAttackState(OwnerComp);
	FinishLatentTask(*OwnerComp, bInterrupted ? EBTNodeResult::Failed : EBTNodeResult::Succeeded);
}

void UBTTask_BossAction::OnSafetyTimeout(UBehaviorTreeComponent* OwnerComp)
{
	if (!OwnerComp || !bTaskActive)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[BTTask_BossAction] Safety timeout - forcing cleanup"));
	CleanupAttackState(OwnerComp);
	FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
}

void UBTTask_BossAction::CleanupAttackState(UBehaviorTreeComponent* OwnerComp)
{
	bTaskActive = false;

	// Safety timer 정리
	if (AAIController* TimerAIC = OwnerComp->GetAIOwner())
	{
		if (APawn* TimerPawn = TimerAIC->GetPawn())
		{
			TimerPawn->GetWorld()->GetTimerManager().ClearTimer(SafetyTimerHandle);
		}
	}

	UBlackboardComponent* BB = OwnerComp->GetBlackboardComponent();
	if (BB)
	{
		BB->SetValueAsBool(NAME_IsAttacking, false);
	}

	if (AAIController* AIC = OwnerComp->GetAIOwner())
	{
		if (APawn* Pawn = AIC->GetPawn())
		{
			if (ABossMonsterBase* Boss = Cast<ABossMonsterBase>(Pawn))
			{
				Boss->ClearWarpTarget(FName("AttackTarget"));
				Boss->RestoreMovementIfAlive();
				Boss->SetSuperArmor(false);
				Boss->SetIdleSoundActive(true);
			}
			else if (ACharacter* Char = Cast<ACharacter>(Pawn))
			{
				Char->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
			}

			if (UMonsterStatComponent* StatComp = Pawn->FindComponentByClass<UMonsterStatComponent>())
			{
				StatComp->MarkAttackUsed();
				StatComp->ResetDamageMultiplier();
			}

			// 보스는 슬롯을 사용하지 않지만, 혹시 있으면 RefreshSlotAfterAttack
			if (BB)
			{
				if (UWorld* World = Pawn->GetWorld())
				{
					if (UMonsterEngagementSubsystem* Engagement = World->GetSubsystem<UMonsterEngagementSubsystem>())
					{
						AActor* Target = Cast<AActor>(BB->GetValueAsObject(NAME_TargetActor));
						if (Target)
						{
							Engagement->RefreshSlotAfterAttack(Pawn, Target);
						}
					}
				}
			}
		}
	}
}

void UBTTask_BossAction::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	if (bTaskActive)
	{
		CleanupAttackState(&OwnerComp);
	}

	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}
