// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_CheckLeash.generated.h"

// 스폰 위치와의 거리 체크 (LeashDistance 초과 시 귀환)
UCLASS()
class NECROMANCER_API UBTService_CheckLeash : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_CheckLeash();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category = "Leash")
	float LeashDistance = 2000.0f;

	UPROPERTY(EditAnywhere, Category = "Leash")
	float ReturnAcceptanceRadius = 100.0f;

private:
	bool bSpawnLocationSaved = false;
};
