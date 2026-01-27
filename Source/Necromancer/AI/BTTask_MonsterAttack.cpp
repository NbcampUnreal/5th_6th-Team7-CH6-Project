// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_MonsterAttack.h"
#include "AIController.h"
#include "GameFramework/Character.h"

UBTTask_MonsterAttack::UBTTask_MonsterAttack()
{
	NodeName = "Monster Attack";
}

EBTNodeResult::Type UBTTask_MonsterAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIC = OwnerComp.GetAIOwner();
	if (!AIC)
	{
		return EBTNodeResult::Failed;
	}
	
	ACharacter* Character = Cast<ACharacter>(AIC->GetPawn());
	if (!Character)
	{
		return EBTNodeResult::Failed;
	}
	
	if (AttackMontage)
	{
		Character->PlayAnimMontage(AttackMontage);
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
	
	
}
