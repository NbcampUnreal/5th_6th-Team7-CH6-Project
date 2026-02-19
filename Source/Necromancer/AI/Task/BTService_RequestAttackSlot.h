// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_RequestAttackSlot.generated.h"

// 공격 슬롯 요청/반환 (MonsterEngagementSubsystem 연동)
UCLASS()
class NECROMANCER_API UBTService_RequestAttackSlot : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_RequestAttackSlot();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual void OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	UPROPERTY()
	TWeakObjectPtr<AActor> CachedTarget;
};
