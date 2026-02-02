// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_RotateToTarget.generated.h"

/**
 * 
 */
UCLASS()
class NECROMANCER_API UBTTask_RotateToTarget : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTTask_RotateToTarget();
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
protected:
	//회전 속도
	UPROPERTY(EditAnywhere, Category ="Roration")
	float InterpSpeed = 6.0;
	//회전 허용 오차
	UPROPERTY(EditAnywhere, Category ="Roration")
	float FinishTolerance = 2.0f;
};
