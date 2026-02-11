// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "EnvQueryContext_Player.generated.h"

/**
 * EQS Context: Blackboard의 TargetActor를 EQS Context로 제공
 * 모든 EQS Query에서 "플레이어 위치"를 참조할 때 사용
 */
UCLASS()
class NECROMANCER_API UEnvQueryContext_Player : public UEnvQueryContext
{
	GENERATED_BODY()

	virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const override;
};
