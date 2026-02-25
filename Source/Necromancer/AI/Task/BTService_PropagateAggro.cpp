// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_PropagateAggro.h"
#include "Necromancer.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTService_PropagateAggro::UBTService_PropagateAggro()
{
	NodeName = "Propagate Aggro";
	Interval = 0.5f;
	RandomDeviation = 0.1f;
}

void UBTService_PropagateAggro::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB)
	{
		return;
	}

	// 이미 사용했으면 스킵
	if (!BB->GetValueAsBool(NAME_CanPropagateAggro))
	{
		return;
	}

	AActor* Target = Cast<AActor>(BB->GetValueAsObject(NAME_TargetActor));
	if (!Target)
	{
		return;
	}

	AAIController* AIC = OwnerComp.GetAIOwner();
	if (!AIC)
	{
		return;
	}

	APawn* OwnerPawn = AIC->GetPawn();
	if (!OwnerPawn || !OwnerPawn->HasAuthority())
	{
		return;
	}

	APawn* TargetPawn = Cast<APawn>(Target);
	if (!TargetPawn)
	{
		return;
	}

	// MakeNoise: Instigator를 플레이어로 설정하여 다른 몬스터가 적으로 인식
	OwnerPawn->MakeNoise(1.0f, TargetPawn, OwnerPawn->GetActorLocation(), AggroRange, NAME_None);

	// 사용 완료 표시
	BB->SetValueAsBool(NAME_CanPropagateAggro, false);
}
