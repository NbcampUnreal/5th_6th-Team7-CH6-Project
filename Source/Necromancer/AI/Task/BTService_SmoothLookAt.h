// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_SmoothLookAt.generated.h"

/**
 * 전투 중 타겟을 부드럽게 바라보는 BTService
 * SetFocus 대신 수동으로 회전을 보간하여 자연스러운 회전 구현
 */
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
