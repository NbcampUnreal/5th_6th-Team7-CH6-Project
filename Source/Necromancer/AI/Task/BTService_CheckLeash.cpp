// Fill out your copyright notice in the Description page of Project Settings.

#include "BTService_CheckLeash.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Necromancer.h"

UBTService_CheckLeash::UBTService_CheckLeash()
{
	NodeName = "Check Leash Distance";
	Interval = 0.5f;
	RandomDeviation = 0.1f;
}

void UBTService_CheckLeash::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB)
	{
		return;
	}

	AAIController* AIC = OwnerComp.GetAIOwner();
	if (!AIC)
	{
		return;
	}

	APawn* Pawn = AIC->GetPawn();
	if (!Pawn)
	{
		return;
	}

	FVector SpawnLocation = BB->GetValueAsVector(FName(NAME_SpawnLocation));
	FVector CurrentLocation = Pawn->GetActorLocation();
	float DistanceFromSpawn = FVector::Dist(CurrentLocation, SpawnLocation);

	bool bShouldReturn = BB->GetValueAsBool(FName(NAME_ShouldReturnToSpawn));

	if (!bShouldReturn)
	{
		// 스폰 위치에서 너무 멀어졌으면 귀환 시작
		if (DistanceFromSpawn > LeashDistance)
		{
			BB->SetValueAsBool(FName(NAME_ShouldReturnToSpawn), true);
			BB->SetValueAsObject(FName(NAME_TargetActor), nullptr);
		}
	}
	else
	{
		// 귀환 중 - 스폰 위치에 도착했으면 귀환 종료
		if (DistanceFromSpawn <= ReturnAcceptanceRadius)
		{
			BB->SetValueAsBool(FName(NAME_ShouldReturnToSpawn), false);
		}
	}
}
