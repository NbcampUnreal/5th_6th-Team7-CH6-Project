// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterAIController.h"

#include "Necromancer.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GenericTeamAgentInterface.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISense_Hearing.h"
#include "GameFramework/PlayerState.h"
#include "Component/StatComponent.h"



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
		AActor* OldTarget = Cast<AActor>(BB->GetValueAsObject(NAME_TargetActor));
		BB->SetValueAsObject(NAME_TargetActor, NewTarget);

		// 타겟이 변경되면 즉시 네트워크 업데이트 → 클라이언트에 회전 변경 즉각 반영
		if (OldTarget != NewTarget)
		{
			if (APawn* MyPawn = GetPawn())
			{
				MyPawn->ForceNetUpdate();
			}
		}
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

void AMonsterAIController::SetLastLocation(FVector LastLocation)
{
	if (UBlackboardComponent* BB = GetBlackboardComponent())
	{
		BB -> SetValueAsVector(NAME_LastLocation,LastLocation);
	}
}

FVector AMonsterAIController::GetLastLocation() const
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
			// 죽은 플레이어는 타겟으로 잡지 않음
			if (APawn* CandidatePawn = Cast<APawn>(TargetCandidate))
			{
				if (APlayerState* PS = CandidatePawn->GetPlayerState())
				{
					if (UStatComponent* StatComp = PS->FindComponentByClass<UStatComponent>())
					{
						if (StatComp->GetStatus() != ECharacterStatus::Alive)
						{
							return;
						}
					}
				}
			}

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
		SetLastLocation(Actor->GetActorLocation());
	}
}

