// Fill out your copyright notice in the Description page of Project Settings.

#include "MonsterStateTreeTasks.h"
#include "MonsterAIController.h"
#include "GAS/Monster/MonsterBase.h"
#include "GAS/Monster/MonsterAttributeSet.h"
#include "GAS/Monster/MonsterGameplayTags.h"
#include "GAS/Base/BaseGameplayTags.h"
#include "StateTreeExecutionContext.h"
#include "AbilitySystemComponent.h"
#include "Navigation/PathFollowingComponent.h"

// ============================================================================
// Helper Functions
// ============================================================================

namespace MonsterStateTreeHelpers
{
	AMonsterAIController* GetAIController(FStateTreeExecutionContext& Context)
	{
		return Cast<AMonsterAIController>(Context.GetOwner());
	}

	APawn* GetPawn(FStateTreeExecutionContext& Context)
	{
		if (AMonsterAIController* AIController = GetAIController(Context))
		{
			return AIController->GetPawn();
		}
		return nullptr;
	}

	UAbilitySystemComponent* GetASC(FStateTreeExecutionContext& Context)
	{
		if (APawn* Pawn = GetPawn(Context))
		{
			return Pawn->FindComponentByClass<UAbilitySystemComponent>();
		}
		return nullptr;
	}
}

// ============================================================================
// Conditions
// ============================================================================

bool FSTCondition_HasGameplayTag::TestCondition(FStateTreeExecutionContext& Context) const
{
	if (!TagToCheck.IsValid())
	{
		return bInvert;
	}

	UAbilitySystemComponent* ASC = MonsterStateTreeHelpers::GetASC(Context);
	if (!ASC)
	{
		return bInvert;
	}

	bool bHasTag = ASC->HasMatchingGameplayTag(TagToCheck);
	return bInvert ? !bHasTag : bHasTag;
}

bool FSTCondition_HasTarget::TestCondition(FStateTreeExecutionContext& Context) const
{
	AMonsterAIController* AIController = MonsterStateTreeHelpers::GetAIController(Context);
	if (!AIController)
	{
		return false;
	}

	return AIController->GetCurrentTarget() != nullptr;
}

bool FSTCondition_IsTargetInAttackRange::TestCondition(FStateTreeExecutionContext& Context) const
{
	AMonsterAIController* AIController = MonsterStateTreeHelpers::GetAIController(Context);
	if (!AIController)
	{
		return false;
	}

	AActor* Target = AIController->GetCurrentTarget();
	if (!Target)
	{
		return false;
	}

	APawn* Pawn = AIController->GetPawn();
	if (!Pawn)
	{
		return false;
	}

	// MonsterBase에서 AttackRange 가져오기
	float AttackRange = 150.0f; // 기본값
	if (AMonsterBase* Monster = Cast<AMonsterBase>(Pawn))
	{
		if (UMonsterAttributeSet* AttributeSet = Monster->GetMonsterAttributeSet())
		{
			AttackRange = AttributeSet->GetAttackRange();
		}
	}

	float Distance = FVector::Dist(Pawn->GetActorLocation(), Target->GetActorLocation());
	return Distance <= AttackRange;
}

bool FSTCondition_HasLastKnownLocation::TestCondition(FStateTreeExecutionContext& Context) const
{
	AMonsterAIController* AIController = MonsterStateTreeHelpers::GetAIController(Context);
	if (!AIController)
	{
		return false;
	}

	// 타겟이 없고 마지막 위치가 있으면 true
	return AIController->GetCurrentTarget() == nullptr && AIController->HasLastKnownLocation();
}

// ============================================================================
// Tasks
// ============================================================================

// ----- FSTTask_ActivateAbilityByTag -----

EStateTreeRunStatus FSTTask_ActivateAbilityByTag::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	if (!AbilityTag.IsValid())
	{
		return EStateTreeRunStatus::Failed;
	}

	UAbilitySystemComponent* ASC = MonsterStateTreeHelpers::GetASC(Context);
	if (!ASC)
	{
		return EStateTreeRunStatus::Failed;
	}

	// 태그로 Ability 활성화
	FGameplayTagContainer AbilityTags;
	AbilityTags.AddTag(AbilityTag);

	bool bActivated = ASC->TryActivateAbilitiesByTag(AbilityTags);
	if (!bActivated)
	{
		// 활성화 실패 (쿨다운, 차단 태그 등)
		return EStateTreeRunStatus::Failed;
	}

	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FSTTask_ActivateAbilityByTag::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	// RunningStateTag로 Ability 실행 상태 확인
	if (!RunningStateTag.IsValid())
	{
		// RunningStateTag가 없으면 한 번 실행 후 성공
		return EStateTreeRunStatus::Succeeded;
	}

	UAbilitySystemComponent* ASC = MonsterStateTreeHelpers::GetASC(Context);
	if (!ASC)
	{
		return EStateTreeRunStatus::Failed;
	}

	// ActivationOwnedTags에 설정된 태그가 있으면 아직 실행 중
	if (ASC->HasMatchingGameplayTag(RunningStateTag))
	{
		return EStateTreeRunStatus::Running;
	}

	// 태그가 없으면 Ability 종료됨
	return EStateTreeRunStatus::Succeeded;
}

// ----- FSTTask_MoveToTarget -----

EStateTreeRunStatus FSTTask_MoveToTarget::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	AMonsterAIController* AIController = MonsterStateTreeHelpers::GetAIController(Context);
	if (!AIController)
	{
		return EStateTreeRunStatus::Failed;
	}

	AActor* Target = AIController->GetCurrentTarget();
	if (!Target)
	{
		return EStateTreeRunStatus::Failed;
	}

	EPathFollowingRequestResult::Type Result = AIController->MoveToActor(Target, AcceptanceRadius);

	if (Result == EPathFollowingRequestResult::Failed)
	{
		return EStateTreeRunStatus::Failed;
	}

	if (Result == EPathFollowingRequestResult::AlreadyAtGoal)
	{
		return EStateTreeRunStatus::Succeeded;
	}

	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FSTTask_MoveToTarget::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	AMonsterAIController* AIController = MonsterStateTreeHelpers::GetAIController(Context);
	if (!AIController)
	{
		return EStateTreeRunStatus::Failed;
	}

	AActor* Target = AIController->GetCurrentTarget();
	if (!Target)
	{
		AIController->StopMovement();
		return EStateTreeRunStatus::Failed;
	}

	EPathFollowingStatus::Type Status = AIController->GetMoveStatus();

	switch (Status)
	{
	case EPathFollowingStatus::Idle:
		{
			// 이동 재시도
			EPathFollowingRequestResult::Type Result = AIController->MoveToActor(Target, AcceptanceRadius);
			if (Result == EPathFollowingRequestResult::AlreadyAtGoal)
			{
				return EStateTreeRunStatus::Succeeded;
			}
		}
		return EStateTreeRunStatus::Running;

	case EPathFollowingStatus::Moving:
		return EStateTreeRunStatus::Running;

	case EPathFollowingStatus::Paused:
		return EStateTreeRunStatus::Running;

	default:
		return EStateTreeRunStatus::Failed;
	}
}

void FSTTask_MoveToTarget::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	if (AMonsterAIController* AIController = MonsterStateTreeHelpers::GetAIController(Context))
	{
		AIController->StopMovement();
	}
}

// ----- FSTTask_MoveToLastKnownLocation -----

EStateTreeRunStatus FSTTask_MoveToLastKnownLocation::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	AMonsterAIController* AIController = MonsterStateTreeHelpers::GetAIController(Context);
	if (!AIController)
	{
		return EStateTreeRunStatus::Failed;
	}

	if (!AIController->HasLastKnownLocation())
	{
		return EStateTreeRunStatus::Failed;
	}

	FVector TargetLocation = AIController->GetLastKnownTargetLocation();
	EPathFollowingRequestResult::Type Result = AIController->MoveToLocation(TargetLocation, AcceptanceRadius);

	if (Result == EPathFollowingRequestResult::Failed)
	{
		AIController->ClearLastKnownLocation();
		return EStateTreeRunStatus::Failed;
	}

	if (Result == EPathFollowingRequestResult::AlreadyAtGoal)
	{
		AIController->ClearLastKnownLocation();
		return EStateTreeRunStatus::Succeeded;
	}

	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FSTTask_MoveToLastKnownLocation::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	AMonsterAIController* AIController = MonsterStateTreeHelpers::GetAIController(Context);
	if (!AIController)
	{
		return EStateTreeRunStatus::Failed;
	}

	// 타겟이 다시 발견되면 즉시 종료 (Chase로 전환)
	if (AIController->GetCurrentTarget())
	{
		return EStateTreeRunStatus::Succeeded;
	}

	EPathFollowingStatus::Type Status = AIController->GetMoveStatus();

	switch (Status)
	{
	case EPathFollowingStatus::Idle:
		// 도착함 - 마지막 위치 클리어
		AIController->ClearLastKnownLocation();
		return EStateTreeRunStatus::Succeeded;

	case EPathFollowingStatus::Moving:
		return EStateTreeRunStatus::Running;

	case EPathFollowingStatus::Paused:
		return EStateTreeRunStatus::Running;

	default:
		AIController->ClearLastKnownLocation();
		return EStateTreeRunStatus::Failed;
	}
}

void FSTTask_MoveToLastKnownLocation::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	if (AMonsterAIController* AIController = MonsterStateTreeHelpers::GetAIController(Context))
	{
		AIController->StopMovement();
	}
}

// ----- FSTTask_ClearCombatState -----

EStateTreeRunStatus FSTTask_ClearCombatState::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	AMonsterAIController* AIController = MonsterStateTreeHelpers::GetAIController(Context);
	if (!AIController)
	{
		return EStateTreeRunStatus::Succeeded;
	}

	// Combat 상태 정리
	AIController->ClearLastKnownLocation();

	// Combat 태그 제거
	if (UAbilitySystemComponent* ASC = MonsterStateTreeHelpers::GetASC(Context))
	{
		const FMonsterGameplayTags& Tags = FMonsterGameplayTags::Get();
		ASC->RemoveLooseGameplayTag(Tags.Monster_State_Combat);
	}

	return EStateTreeRunStatus::Succeeded;
}
