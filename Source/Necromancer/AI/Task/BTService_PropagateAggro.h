// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_PropagateAggro.generated.h"

UCLASS()
class NECROMANCER_API UBTService_PropagateAggro : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_PropagateAggro();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	// MakeNoise 전파 범위
	UPROPERTY(EditAnywhere, Category = "Aggro")
	float AggroRange = 1500.0f;
};
