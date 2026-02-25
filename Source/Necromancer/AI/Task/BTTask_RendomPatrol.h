// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_RendomPatrol.generated.h"

/**
 * 
 */
UCLASS()
class NECROMANCER_API UBTTask_RendomPatrol : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTTask_RendomPatrol();
	
protected:
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	//스폰위치 기준 패트롤 반경
	UPROPERTY(EditAnywhere, Category = "Patrol")
	float PatrolRadius = 1000.0f;
	
};
