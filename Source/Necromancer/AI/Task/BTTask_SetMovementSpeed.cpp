// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_SetMovementSpeed.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UBTTask_SetMovementSpeed::UBTTask_SetMovementSpeed()
{
	NodeName = TEXT("Set Movement Speed");
	bNotifyTick = false;
}

EBTNodeResult::Type UBTTask_SetMovementSpeed::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	Character->GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
	return EBTNodeResult::Succeeded;
}

FString UBTTask_SetMovementSpeed::GetStaticDescription() const
{
	return FString::Printf(TEXT("Speed: %.0f"), MaxWalkSpeed);
}
