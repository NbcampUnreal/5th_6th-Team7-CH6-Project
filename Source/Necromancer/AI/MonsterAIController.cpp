// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterAIController.h"

#include "Necromancer.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GenericTeamAgentInterface.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISense_Hearing.h"



AMonsterAIController::AMonsterAIController()
{
	// 몬스터 팀 설정 (Perception 친적 판별에 필요)
	SetGenericTeamId(FGenericTeamId(TEAM_ID_MONSTER));

	AIPerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>("AIPerceptionComp");
	SetPerceptionComponent(*AIPerceptionComp);

	// Hearing Sense 설정 (어그로 전파 수신용)
	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
	HearingConfig->HearingRange = HearingRange;
	HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
	HearingConfig->DetectionByAffiliation.bDetectNeutrals = false;
	HearingConfig->DetectionByAffiliation.bDetectFriendlies = false;
	AIPerceptionComp->ConfigureSense(*HearingConfig);
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

	// 타겟 해제 후 30초 뒤 어그로 전파 재사용 가능
	GetWorldTimerManager().ClearTimer(AggroResetTimerHandle);
	GetWorldTimerManager().SetTimer(AggroResetTimerHandle, this, &AMonsterAIController::ResetAggroPropagation, AggroResetTime, false);
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


void AMonsterAIController::ResetAggroPropagation()
{
	if (UBlackboardComponent* BB = GetBlackboardComponent())
	{
		BB->SetValueAsBool(NAME_CanPropagateAggro, true);
	}
}

void AMonsterAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (Stimulus.WasSuccessfullySensed())
	{
		GetWorldTimerManager().ClearTimer(AggroResetTimerHandle);

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
				BB->SetValueAsBool(NAME_ShouldReturnToSpawn, false);
			}
		}
	}
	else
	{
		// 감지를 잃어도 타겟을 유지 (귀환/사망 시에만 해제)
		// 마지막 위치만 기록
		SetlastLocation(Actor->GetActorLocation());
	}
}

