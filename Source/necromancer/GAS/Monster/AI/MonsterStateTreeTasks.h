// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "StateTreeConditionBase.h"
#include "MonsterStateTreeTasks.generated.h"

class AMonsterBase;
class UMonsterAttributeSet;
class AMonsterAIController;

// ======================== Instance Data ========================

USTRUCT()
struct FSTCondition_IsPlayerInRangeInstanceData
{
	GENERATED_BODY()
};

USTRUCT()
struct FSTCondition_IsPlayerOutOfRangeInstanceData
{
	GENERATED_BODY()
};

USTRUCT()
struct FSTCondition_IsPlayerInAttackRangeInstanceData
{
	GENERATED_BODY()
};

USTRUCT()
struct FSTCondition_IsMonsterDeadInstanceData
{
	GENERATED_BODY()
};

USTRUCT()
struct FSTTask_MoveToTargetInstanceData
{
	GENERATED_BODY()
};

USTRUCT()
struct FSTTask_ExecuteMeleeAttackInstanceData
{
	GENERATED_BODY()
};

// ======================== Conditions ========================

USTRUCT(meta = (DisplayName = "플레이어가 감지 범위에 있는지 확인"))
struct NECROMANCER_API FSTCondition_IsPlayerInRange : public FStateTreeConditionCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSTCondition_IsPlayerInRangeInstanceData;

	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	}

	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;
};

USTRUCT(meta = (DisplayName = "플레이어가 감지 범위 밖에 있는지 확인"))
struct NECROMANCER_API FSTCondition_IsPlayerOutOfRange : public FStateTreeConditionCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSTCondition_IsPlayerOutOfRangeInstanceData;

	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	}

	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;
};

USTRUCT(meta = (DisplayName = "플레이어가 공격 범위에 있는지 확인"))
struct NECROMANCER_API FSTCondition_IsPlayerInAttackRange : public FStateTreeConditionCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSTCondition_IsPlayerInAttackRangeInstanceData;

	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	}

	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;
};

USTRUCT(meta = (DisplayName = "몬스터가 죽었는지 확인"))
struct NECROMANCER_API FSTCondition_IsMonsterDead : public FStateTreeConditionCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSTCondition_IsMonsterDeadInstanceData;

	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	}

	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;
};

// ======================== Tasks ========================

USTRUCT(meta = (DisplayName = "타겟으로 이동"))
struct NECROMANCER_API FSTTask_MoveToTarget : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSTTask_MoveToTargetInstanceData;

	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	}

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float AcceptanceRadius = 100.0f;
};

USTRUCT(meta = (DisplayName = "근접 공격 실행"))
struct NECROMANCER_API FSTTask_ExecuteMeleeAttack : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSTTask_ExecuteMeleeAttackInstanceData;

	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	}

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
};
