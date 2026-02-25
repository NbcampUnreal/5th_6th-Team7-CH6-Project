// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_IsAlive.generated.h"

// 몬스터 생존 여부 체크 - 사망 시 서브트리 중단
UCLASS()
class NECROMANCER_API UBTDecorator_IsAlive : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_IsAlive();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	virtual FString GetStaticDescription() const override;
};
