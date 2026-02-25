// Fill out your copyright notice in the Description page of Project Settings.

#include "BTService_RequestAttackSlot.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "MonsterEngagementSubsystem.h"
#include "Necromancer.h"

UBTService_RequestAttackSlot::UBTService_RequestAttackSlot()
{
	NodeName = "Request Attack Slot";
	Interval = 0.3f;
	RandomDeviation = 0.05f;
	bCreateNodeInstance = true;
}

void UBTService_RequestAttackSlot::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
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

	APawn* AIPawn = AIC->GetPawn();
	if (!AIPawn)
	{
		return;
	}

	AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(NAME_TargetActor));
	if (!TargetActor)
	{
		BB->SetValueAsBool(NAME_HasAttackSlot, false);
		return;
	}

	
	CachedTarget = TargetActor;

	UMonsterEngagementSubsystem* Engagement = AIPawn->GetWorld()->GetSubsystem<UMonsterEngagementSubsystem>();
	if (!Engagement)
	{
		BB->SetValueAsBool(NAME_HasAttackSlot, false);
		return;
	}

	bool bHasSlot = Engagement->RequestAttackSlot(AIPawn, TargetActor);

	// 슬롯 유지 시간 초과 + 만석이면 양보
	if (bHasSlot && Engagement->ShouldYieldSlot(AIPawn, TargetActor))
	{
		Engagement->ReleaseAttackSlot(AIPawn, TargetActor);
		bHasSlot = false;
	}

	BB->SetValueAsBool(NAME_HasAttackSlot, bHasSlot);
}

void UBTService_RequestAttackSlot::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnCeaseRelevant(OwnerComp, NodeMemory);

	AAIController* AIC = OwnerComp.GetAIOwner();
	if (!AIC)
	{
		return;
	}

	APawn* AIPawn = AIC->GetPawn();
	if (!AIPawn)
	{
		return;
	}

	
	if (CachedTarget.IsValid())
	{
		UMonsterEngagementSubsystem* Engagement = AIPawn->GetWorld()->GetSubsystem<UMonsterEngagementSubsystem>();
		if (Engagement)
		{
			Engagement->ReleaseAttackSlot(AIPawn, CachedTarget.Get());
		}
		CachedTarget.Reset();
	}

	
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (BB)
	{
		BB->SetValueAsBool(NAME_HasAttackSlot, false);
	}
}
