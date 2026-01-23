// Fill out your copyright notice in the Description page of Project Settings.

#include "MonsterStateTreeTasks.h"
#include "MonsterAIController.h"
#include "MonsterBase.h"
#include "MonsterAttributeSet.h"
#include "StateTreeExecutionContext.h"
#include "AbilitySystemComponent.h"
#include "MonsterCombatInterface.h"
#include "GameplayTagContainer.h"
#include "MonsterGameplayTags.h"
#include "Navigation/PathFollowingComponent.h"
#include "BaseGameplayTags.h"

bool FSTCondition_IsPlayerInRange::TestCondition(FStateTreeExecutionContext& Context) const
{
	AMonsterAIController* AIController = Cast<AMonsterAIController>(Context.GetOwner());
	if (!AIController)
	{
		return false;
	}
	AActor* Target = AIController->GetCurrentTarget();
	return Target != nullptr;
}

bool FSTCondition_IsPlayerInAttackRange::TestCondition(FStateTreeExecutionContext& Context) const
{
	AMonsterAIController* AIController = Cast<AMonsterAIController>(Context.GetOwner());
	if (!AIController)
	{
		return false;
	}

	AActor* Target = AIController->GetCurrentTarget();
	if (!Target)
	{
		return false;
	}

	APawn* ControlledPawn = AIController->GetPawn();
	if (!ControlledPawn)
	{
		return false;
	}

	// MonsterBase에서 AttributeSet 가져오기
	AMonsterBase* Monster = Cast<AMonsterBase>(ControlledPawn);
	if (!Monster)
	{
		return false;
	}

	UMonsterAttributeSet* AttributeSet = Monster->GetMonsterAttributeSet();
	if (!AttributeSet)
	{
		return false;
	}

	// 거리 계산
	float Distance = FVector::Dist(ControlledPawn->GetActorLocation(), Target->GetActorLocation());
	float AttackRange = AttributeSet->GetAttackRange();

	return Distance <= AttackRange;
}

EStateTreeRunStatus FSTTask_ExecuteMeleeAttack::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
  {
        AMonsterAIController* AIController = Cast<AMonsterAIController>(Context.GetOwner());
        if (!AIController)
        {
                return EStateTreeRunStatus::Failed;
        }

        APawn* ControlledPawn = AIController->GetPawn();
        if (!ControlledPawn)
        {
                return EStateTreeRunStatus::Failed;
        }

        // IMonsterCombatInterface를 통해 공격 가능 여부 확인 후 공격 실행
        if (ControlledPawn->Implements<UMonsterCombatInterface>())
        {
                bool bCanAttack = IMonsterCombatInterface::Execute_CanAttack(ControlledPawn);
                if (bCanAttack)
                {
                        // ASC에서 공격 어빌리티 활성화
                        if (UAbilitySystemComponent* ASC = ControlledPawn->FindComponentByClass<UAbilitySystemComponent>())
                        {
                                // 태그로 공격 어빌리티 활성화
                        	const FMonsterGameplayTags& Tags = FMonsterGameplayTags::Get();
                        	FGameplayTagContainer AttackTags;
                        	AttackTags.AddTag(Tags.Monster_Ability_Attack_Melee);
                        	ASC->TryActivateAbilitiesByTag(AttackTags);
                        }
                }
        }

        return EStateTreeRunStatus::Running;
  }

  EStateTreeRunStatus FSTTask_ExecuteMeleeAttack::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
  {
        AMonsterAIController* AIController = Cast<AMonsterAIController>(Context.GetOwner());
        if (!AIController)
        {
                return EStateTreeRunStatus::Failed;
        }

        APawn* ControlledPawn = AIController->GetPawn();
        if (!ControlledPawn || !ControlledPawn->Implements<UMonsterCombatInterface>())
        {
                return EStateTreeRunStatus::Failed;
        }

        
        bool bCanAttack = IMonsterCombatInterface::Execute_CanAttack(ControlledPawn);
        if (bCanAttack)
        {
                return EStateTreeRunStatus::Succeeded;
        }

        return EStateTreeRunStatus::Running;
  }

  void FSTTask_ExecuteMeleeAttack::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
  {
        
  }

  

bool FSTCondition_IsMonsterDead::TestCondition(FStateTreeExecutionContext& Context) const
{
	AMonsterAIController* AIController = Cast<AMonsterAIController>(Context.GetOwner());
	if (!AIController)
	{
		return false;
	}

	APawn* ControlledPawn = AIController->GetPawn();
	if (!ControlledPawn)
	{
		return true; // Pawn이 없으면 죽은 것으로 간주
	}

	// 태그로 확인
	if (UAbilitySystemComponent* ASC = ControlledPawn->FindComponentByClass<UAbilitySystemComponent>())
	{
		const FBaseGameplayTags& BaseTags = FBaseGameplayTags::Get();
		return ASC->HasMatchingGameplayTag(BaseTags.State_Dead);
	}

	return false;
}

EStateTreeRunStatus FSTTask_MoveToTarget::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
  {
        AMonsterAIController* AIController = Cast<AMonsterAIController>(Context.GetOwner());
        if (!AIController)
        {
                return EStateTreeRunStatus::Failed;
        }

        AActor* Target = AIController->GetCurrentTarget();
        if (!Target)
        {
                return EStateTreeRunStatus::Failed;
        }

        // MoveTo 시작
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
        AMonsterAIController* AIController = Cast<AMonsterAIController>(Context.GetOwner());
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
        AMonsterAIController* AIController = Cast<AMonsterAIController>(Context.GetOwner());
        if (AIController)
        {
                AIController->StopMovement();
        }
  }

bool FSTCondition_IsPlayerOutOfRange::TestCondition(FStateTreeExecutionContext& Context) const
{
	AMonsterAIController* AIController = Cast<AMonsterAIController>(Context.GetOwner());
	if (!AIController)
	{
		return true; // 컨트롤러 없으면 범위 밖으로 간주
	}
	AActor* Target = AIController->GetCurrentTarget();
	return Target == nullptr; // 타겟이 없으면 true
}