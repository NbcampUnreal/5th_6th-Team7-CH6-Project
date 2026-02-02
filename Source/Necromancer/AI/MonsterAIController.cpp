// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterAIController.h"

#include "Necromancer.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GenericTeamAgentInterface.h"
#include "Perception/AIPerceptionComponent.h"


// Sets default values
AMonsterAIController::AMonsterAIController()
{
	AIPerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>("AIPerceptionComp");
	SetPerceptionComponent(*AIPerceptionComp);
	
}

void AMonsterAIController::BeginPlay()
{
	Super::BeginPlay();
	
	if (!HasAuthority())
	{
		return;
	}
	
	if (AIPerceptionComp)
	{
		AIPerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &AMonsterAIController::OnTargetPerceptionUpdated);
		
	}
	// 비헤이비어 트리 설정 안해도 경고만 뜨게 만드는 방법
	if (ensureMsgf(BehaviorTree, TEXT("BehaviorTree is nullptr")))
	{
		RunBehaviorTree(BehaviorTree);
	}
	
	
}
void AMonsterAIController::SetTargetActor(AActor* NewTarget)
{
	if (UBlackboardComponent* BB = GetBlackboardComponent())
	{
		BB -> SetValueAsObject(NAME_TargetActor, NewTarget);
	}
}

AActor* AMonsterAIController::GetTargetActor() const
{
	if (const UBlackboardComponent* BB = GetBlackboardComponent())
	{
		return Cast<AActor>(BB->GetValueAsObject(NAME_TargetActor));
	}
	return nullptr;
}

void AMonsterAIController::ClearTargetActor()
{
	SetTargetActor(nullptr);
}

void AMonsterAIController::SetlastLocation(FVector LastLocation)
{
	if (UBlackboardComponent* BB = GetBlackboardComponent())
	{
		BB -> SetValueAsVector(NAME_LastLocation,LastLocation);
	}
}

FVector AMonsterAIController::GetlastLocation() const
{
	if (const UBlackboardComponent* BB = GetBlackboardComponent())
	{
		return BB->GetValueAsVector(NAME_LastLocation);
	}
	return FVector::ZeroVector;
}


void AMonsterAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (Stimulus.WasSuccessfullySensed())
	{
		GetWorldTimerManager().ClearTimer(LoseTargetTimerHandle);
		IGenericTeamAgentInterface* GenericTeamAgentInterface = Cast<IGenericTeamAgentInterface>(Actor);
		if (GenericTeamAgentInterface && GenericTeamAgentInterface->GetGenericTeamId() == FGenericTeamId(TEAM_ID_PLAYER))
		{
			SetTargetActor(Actor);
		}
		
	}
	else
	{
		SetlastLocation(Actor->GetActorLocation());
		
		GetWorldTimerManager().SetTimer(LoseTargetTimerHandle, this,&AMonsterAIController::ClearTargetActor,ClearTime,false);
		
	
	}
}


