// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_CheckLeash.generated.h"

UCLASS()
class NECROMANCER_API UBTService_CheckLeash : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_CheckLeash();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	// 스폰 위치에서 이 거리 이상 벗어나면 귀환
	UPROPERTY(EditAnywhere, Category = "Leash")
	float LeashDistance = 2000.0f;

	// 스폰 위치 도착 판정 거리
	UPROPERTY(EditAnywhere, Category = "Leash")
	float ReturnAcceptanceRadius = 100.0f;
};
