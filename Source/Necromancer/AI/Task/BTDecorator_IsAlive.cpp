// Fill out your copyright notice in the Description page of Project Settings.

#include "BTDecorator_IsAlive.h"
#include "AIController.h"
#include "MonsterBase.h"

UBTDecorator_IsAlive::UBTDecorator_IsAlive()
{
	NodeName = "Is Alive";
}

bool UBTDecorator_IsAlive::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	AAIController* AIC = OwnerComp.GetAIOwner();
	if (!AIC)
	{
		return false;
	}

	AMonsterBase* Monster = Cast<AMonsterBase>(AIC->GetPawn());
	if (!Monster)
	{
		return false;
	}

	return !Monster->GetIsDead();
}

FString UBTDecorator_IsAlive::GetStaticDescription() const
{
	return TEXT("Monster is alive (not dead)");
}
