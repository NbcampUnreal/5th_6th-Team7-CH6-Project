// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_CheckAttackRange.generated.h"

// 타겟과의 거리를 체크하여 공격 범위 BB키 갱신
UCLASS()
class NECROMANCER_API UBTService_CheckAttackRange : public UBTService
{
	GENERATED_BODY()
	
public:
	UBTService_CheckAttackRange();
	
protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
	UPROPERTY(EditAnywhere, Category ="AI")
	FBlackboardKeySelector TargetActorKey;
	
	UPROPERTY(EditAnywhere, Category ="AI")
	FBlackboardKeySelector AttackRangeKey;
	
	UPROPERTY(EditAnywhere, Category ="AI")
	float MaxAttackRange = 150.0f;
	
};
