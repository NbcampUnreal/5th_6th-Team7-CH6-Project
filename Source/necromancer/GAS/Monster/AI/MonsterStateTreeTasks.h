// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "StateTreeConditionBase.h"
#include "GameplayTagContainer.h"
#include "MonsterStateTreeTasks.generated.h"

class AMonsterAIController;
class UAbilitySystemComponent;

// ============================================================================
// Context Data - StateTree Schema에서 바인딩할 데이터
// ============================================================================

/**
 * 몬스터 AI용 공유 데이터
 * StateTree의 Evaluator나 Global Task에서 한 번 계산하고 다른 Task/Condition에서 재사용
 */
USTRUCT(BlueprintType)
struct NECROMANCER_API FMonsterStateTreeContextData
{
	GENERATED_BODY()

	// AIController에서 가져온 현재 타겟
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AActor> CurrentTarget = nullptr;

	// 타겟과의 거리
	UPROPERTY(EditAnywhere, Category = "Context")
	float DistanceToTarget = 0.0f;

	// 공격 범위 (AttributeSet에서)
	UPROPERTY(EditAnywhere, Category = "Context")
	float AttackRange = 150.0f;

	// 마지막 목격 위치
	UPROPERTY(EditAnywhere, Category = "Context")
	FVector LastKnownLocation = FVector::ZeroVector;

	// 마지막 목격 위치 유효 여부
	UPROPERTY(EditAnywhere, Category = "Context")
	bool bHasLastKnownLocation = false;
};

// ============================================================================
// Condition Instance Data - StateTree가 Condition 상태를 저장하는 구조체
// ============================================================================

USTRUCT()
struct NECROMANCER_API FSTCondition_HasGameplayTagInstanceData
{
	GENERATED_BODY()
};

USTRUCT()
struct NECROMANCER_API FSTCondition_HasTargetInstanceData
{
	GENERATED_BODY()
};

USTRUCT()
struct NECROMANCER_API FSTCondition_IsTargetInAttackRangeInstanceData
{
	GENERATED_BODY()
};

USTRUCT()
struct NECROMANCER_API FSTCondition_HasLastKnownLocationInstanceData
{
	GENERATED_BODY()
};

// ============================================================================
// Conditions - GAS 태그 기반 단순 조건
// ============================================================================

/**
 * ASC에 특정 태그가 있는지 확인
 * 예: State.Dead, Monster.State.Combat, Monster.State.Attacking
 */
USTRUCT(meta = (DisplayName = "Has Gameplay Tag"))
struct NECROMANCER_API FSTCondition_HasGameplayTag : public FStateTreeConditionCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSTCondition_HasGameplayTagInstanceData;

	// 확인할 태그
	UPROPERTY(EditAnywhere, Category = "Condition")
	FGameplayTag TagToCheck;

	// 조건 반전 (태그가 없으면 true)
	UPROPERTY(EditAnywhere, Category = "Condition")
	bool bInvert = false;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;
};

/**
 * 현재 타겟이 있는지 확인
 */
USTRUCT(meta = (DisplayName = "Has Target"))
struct NECROMANCER_API FSTCondition_HasTarget : public FStateTreeConditionCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSTCondition_HasTargetInstanceData;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;
};

/**
 * 타겟이 공격 범위 내에 있는지 확인
 */
USTRUCT(meta = (DisplayName = "Is Target In Attack Range"))
struct NECROMANCER_API FSTCondition_IsTargetInAttackRange : public FStateTreeConditionCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSTCondition_IsTargetInAttackRangeInstanceData;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;
};

/**
 * 마지막 목격 위치가 있는지 확인
 */
USTRUCT(meta = (DisplayName = "Has Last Known Location"))
struct NECROMANCER_API FSTCondition_HasLastKnownLocation : public FStateTreeConditionCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSTCondition_HasLastKnownLocationInstanceData;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;
};

// ============================================================================
// Task Instance Data - StateTree가 Task 상태를 저장하는 구조체
// ============================================================================

USTRUCT()
struct NECROMANCER_API FSTTask_ActivateAbilityByTagInstanceData
{
	GENERATED_BODY()
};

USTRUCT()
struct NECROMANCER_API FSTTask_MoveToTargetInstanceData
{
	GENERATED_BODY()
};

USTRUCT()
struct NECROMANCER_API FSTTask_MoveToLastKnownLocationInstanceData
{
	GENERATED_BODY()
};

USTRUCT()
struct NECROMANCER_API FSTTask_ClearCombatStateInstanceData
{
	GENERATED_BODY()
};

// ============================================================================
// Tasks - 최소한의 커스텀 Task
// ============================================================================

/**
 * GAS Ability 활성화 Task
 * 태그로 Ability를 활성화하고 완료될 때까지 대기
 */
USTRUCT(meta = (DisplayName = "Activate Ability By Tag"))
struct NECROMANCER_API FSTTask_ActivateAbilityByTag : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSTTask_ActivateAbilityByTagInstanceData;

	// 활성화할 Ability의 태그
	UPROPERTY(EditAnywhere, Category = "Ability")
	FGameplayTag AbilityTag;

	// Ability 실행 중임을 나타내는 태그 (ActivationOwnedTags)
	// 이 태그가 없어지면 Ability가 끝난 것으로 판단
	UPROPERTY(EditAnywhere, Category = "Ability")
	FGameplayTag RunningStateTag;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
};

/**
 * 타겟 Actor로 이동
 */
USTRUCT(meta = (DisplayName = "Move To Target"))
struct NECROMANCER_API FSTTask_MoveToTarget : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSTTask_MoveToTargetInstanceData;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float AcceptanceRadius = 100.0f;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
};

/**
 * 특정 위치로 이동 (마지막 목격 위치용)
 */
USTRUCT(meta = (DisplayName = "Move To Last Known Location"))
struct NECROMANCER_API FSTTask_MoveToLastKnownLocation : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSTTask_MoveToLastKnownLocationInstanceData;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float AcceptanceRadius = 50.0f;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
};

/**
 * Combat 상태 정리 (Search 완료 후 Idle로 전환 시)
 */
USTRUCT(meta = (DisplayName = "Clear Combat State"))
struct NECROMANCER_API FSTTask_ClearCombatState : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSTTask_ClearCombatStateInstanceData;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
};

// ============================================================================
// Helper Functions
// ============================================================================

namespace MonsterStateTreeHelpers
{
	// AIController 가져오기
	AMonsterAIController* GetAIController(FStateTreeExecutionContext& Context);

	// Pawn 가져오기
	APawn* GetPawn(FStateTreeExecutionContext& Context);

	// ASC 가져오기
	UAbilitySystemComponent* GetASC(FStateTreeExecutionContext& Context);
}
