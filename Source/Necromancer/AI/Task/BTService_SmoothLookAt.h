// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_SmoothLookAt.generated.h"

// 타겟이 있으면 SetFocus + 전투 이동 모드(Strafe), 없으면 순찰 모드 자동 전환
// 회전 속도는 MonsterBase::CombatRotationSpeed (RotationRate)로 제어
UCLASS()
class NECROMANCER_API UBTService_SmoothLookAt : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_SmoothLookAt();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual void OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
