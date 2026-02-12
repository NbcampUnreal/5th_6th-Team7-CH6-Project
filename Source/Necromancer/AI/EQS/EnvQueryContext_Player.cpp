// Fill out your copyright notice in the Description page of Project Settings.

#include "EnvQueryContext_Player.h"
#include "Necromancer.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

void UEnvQueryContext_Player::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	const AActor* QueryOwner = Cast<AActor>(QueryInstance.Owner.Get());
	if (!QueryOwner)
	{
		return;
	}

	const APawn* OwnerPawn = Cast<APawn>(QueryOwner);
	if (!OwnerPawn)
	{
		return;
	}

	const AAIController* AIC = Cast<AAIController>(OwnerPawn->GetController());
	if (!AIC)
	{
		return;
	}

	const UBlackboardComponent* BB = AIC->GetBlackboardComponent();
	if (!BB)
	{
		return;
	}

	AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(FName(NAME_TargetActor)));
	if (TargetActor)
	{
		UEnvQueryItemType_Actor::SetContextHelper(ContextData, TargetActor);
	}
}
