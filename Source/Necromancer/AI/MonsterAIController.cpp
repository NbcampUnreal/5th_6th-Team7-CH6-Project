// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterAIController.h"

#include "Necromancer.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GenericTeamAgentInterface.h"
#include "Perception/AIPerceptionComponent.h"



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
	
	if (ensureMsgf(BehaviorTree, TEXT("BehaviorTree is nullptr")))
	{
		RunBehaviorTree(BehaviorTree);
	}

}
void AMonsterAIController::SetTargetActor(AActor* NewTarget)
{
	if (UBlackboardComponent* BB = GetBlackboardComponent())
	{
		BB->SetValueAsObject(NAME_TargetActor, NewTarget);
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

		// 타겟 후보 결정: 직접 팀 ID가 없으면 Instigator를 확인
		AActor* TargetCandidate = Actor;
		IGenericTeamAgentInterface* TeamAgent = Cast<IGenericTeamAgentInterface>(Actor);
		if (!TeamAgent)
		{
			APawn* InstigatorPawn = Actor->GetInstigator();
			if (InstigatorPawn)
			{
				TeamAgent = Cast<IGenericTeamAgentInterface>(InstigatorPawn);
				TargetCandidate = InstigatorPawn;
			}
		}

		if (TeamAgent && TeamAgent->GetGenericTeamId() == FGenericTeamId(TEAM_ID_PLAYER))
		{
			SetTargetActor(TargetCandidate);

			// 귀환 취소
			if (UBlackboardComponent* BB = GetBlackboardComponent())
			{
				BB->SetValueAsBool(FName(NAME_ShouldReturnToSpawn), false);
			}
		}
	}
	else
	{
		SetlastLocation(Actor->GetActorLocation());

		GetWorldTimerManager().SetTimer(LoseTargetTimerHandle, this, &AMonsterAIController::ClearTargetActor, ClearTime, false);
	}
}

