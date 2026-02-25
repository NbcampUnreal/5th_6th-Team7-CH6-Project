// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_RendomPatrol.h"
#include "Necromancer.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_RendomPatrol::UBTTask_RendomPatrol()
{
	NodeName = TEXT("Rendom Patrol");
}

EBTNodeResult::Type UBTTask_RendomPatrol::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIC = OwnerComp.GetAIOwner();
	if (!AIC || !AIC -> GetPawn())
	{
		return EBTNodeResult::Failed;
	}
	
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB)
	{
		return EBTNodeResult::Failed;
	}
	
	// 스폰 위치를 기준점으로 사용, 없으면 현재 위치 사용
	FVector Origin = BB->GetValueAsVector(NAME_SpawnLocation);
	if (Origin.IsZero())
	{
		Origin = AIC->GetPawn()->GetActorLocation();
	}
	
	// NavMesh 위에서 랜덤 위치 찾기
	FNavLocation NavLocation;
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!NavSys)
	{
		return EBTNodeResult::Failed;
	}
	
	bool bFound = NavSys->GetRandomReachablePointInRadius(Origin, PatrolRadius, NavLocation);
	if (!bFound)
	{
		return EBTNodeResult::Failed;
	}

	// BB에 이동 목표 위치 설정
	BB->SetValueAsVector(NAME_PatrolLocation, NavLocation.Location);

	return EBTNodeResult::Succeeded;
}
