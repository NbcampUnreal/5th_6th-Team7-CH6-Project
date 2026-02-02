// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_CheckAttackRange.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include  "Necromancer.h"

UBTService_CheckAttackRange::UBTService_CheckAttackRange()
{
	NodeName = "Check Attack Range";
	Interval = 0.5f;
	RandomDeviation = 0.1f;
	
	TargetActorKey.SelectedKeyName = NAME_TargetActor;
}

void UBTService_CheckAttackRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB)
	{
		return;
	}
	
	AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(TargetActorKey.SelectedKeyName));
	if (!TargetActor)
	{
		BB->SetValueAsBool(AttackRangeKey.SelectedKeyName, false);
		return;
	}
	
	AAIController* AIC = OwnerComp.GetAIOwner();
	if (!AIC)
	{
		return;
	}
	
	APawn* AIpawn = AIC->GetPawn();
	if (!AIpawn)
	{
		return;
	}
	
	float Distance = FVector::Dist(AIpawn->GetActorLocation(), TargetActor->GetActorLocation());
	bool bWthinRange = Distance <= MaxAttackRange;
	
	bool bHasLOS = false;
	if (bWthinRange)
	{
		//레이 케스트로 공격 가능 판단.
		bHasLOS = AIC->LineOfSightTo(TargetActor);
	}
	
	BB->SetValueAsBool(AttackRangeKey.SelectedKeyName, bWthinRange && bHasLOS);
	
	
}
