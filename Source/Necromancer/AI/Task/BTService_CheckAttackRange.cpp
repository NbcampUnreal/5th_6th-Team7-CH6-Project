// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_CheckAttackRange.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/PlayerState.h"
#include "MonsterStatComponent.h"
#include "MonsterAIController.h"
#include "Necromancer.h"

UBTService_CheckAttackRange::UBTService_CheckAttackRange()
{
	NodeName = "Check Attack Range";
	Interval = 0.5f;
	RandomDeviation = 0.1f;

	TargetActorKey.SelectedKeyName = NAME_TargetActor;
	AttackRangeKey.SelectedKeyName = NAME_InAttackRange;
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

	// 타겟 플레이어 생존 여부 확인 (Down/Death 시 타겟 해제)
	if (APawn* TargetPawn = Cast<APawn>(TargetActor))
	{
		if (APlayerState* PS = TargetPawn->GetPlayerState())
		{
			if (UStatComponent* TargetStatComp = PS->FindComponentByClass<UStatComponent>())
			{
				if (TargetStatComp->GetStatus() != ECharacterStatus::Alive)
				{
					if (AMonsterAIController* MonsterAIC = Cast<AMonsterAIController>(OwnerComp.GetAIOwner()))
					{
						MonsterAIC->ClearTargetActor();
					}
					BB->SetValueAsBool(AttackRangeKey.SelectedKeyName, false);
					return;
				}
			}
		}
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
	
	
	UMonsterStatComponent* StatComp = AIpawn->FindComponentByClass<UMonsterStatComponent>();
	float EffectiveRange = StatComp ? StatComp->GetAttackRange() : MaxAttackRange;

	float Distance = FVector::Dist(AIpawn->GetActorLocation(), TargetActor->GetActorLocation());
	bool bWithinRange = Distance <= EffectiveRange;

	
	bool bTooClose = false;
	if (StatComp && StatComp->GetIsRanged())
	{
		bTooClose = Distance < StatComp->GetMinAttackRange();
	}
	BB->SetValueAsBool(FName(NAME_IsTooClose), bTooClose);

	bool bHasLOS = false;
	if (bWithinRange)
	{
		
		bHasLOS = AIC->LineOfSightTo(TargetActor);
	}

	bool bFinalResult = bWithinRange && bHasLOS && !bTooClose;
	BB->SetValueAsBool(AttackRangeKey.SelectedKeyName, bFinalResult);
}
