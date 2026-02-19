// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_SmoothLookAt.generated.h"

// 타겟을 향해 부드럽게 회전 (Tick 기반)
UCLASS()
class NECROMANCER_API UBTService_SmoothLookAt : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_SmoothLookAt();

	
	UPROPERTY(EditAnywhere, Category = "Rotation")
	float RotationSpeed = 180.0f;

	
	UPROPERTY(EditAnywhere, Category = "Rotation")
	bool bYawOnly = true;

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
