// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterAIController.h"
#include "Components/StateTreeAIComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "GameFramework/Character.h"


AMonsterAIController::AMonsterAIController()
{
	PrimaryActorTick.bCanEverTick = false;
	
	StateTreeAIComponent = CreateDefaultSubobject<UStateTreeAIComponent>(TEXT("StateTreeAIComponent"));
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
	SetPerceptionComponent(*AIPerceptionComponent);
	
	//시야 감지 범위
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 1000.0f; //감지 범위
	SightConfig->LoseSightRadius = 1200.0f; //시야 소실
	SightConfig->PeripheralVisionAngleDegrees = 90.0f; //각도
	SightConfig->SetMaxAge(5.0f); //감지 정보 유지 시간
	SightConfig->AutoSuccessRangeFromLastSeenLocation = 500.0f;
	
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	
	AIPerceptionComponent->ConfigureSense(*SightConfig);
	AIPerceptionComponent->SetDominantSense(UAISense_Sight::StaticClass());
	
}


void AMonsterAIController::BeginPlay()
{
	Super::BeginPlay();
	SetupPerceptionSystem();
}

void AMonsterAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	UE_LOG(LogTemp, Log, TEXT("MonsterAIController: Possessed %s"), *GetNameSafe(InPawn));
}

void AMonsterAIController::OnUnPossess()
{
	Super::OnUnPossess();

	// 타겟 초기화
	ClearCurrentTarget();
}

void AMonsterAIController::SetupPerceptionSystem()
{
	if (AIPerceptionComponent)
	{
		AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AMonsterAIController::OnTargetPerceptionUpdated);
	}
}

void AMonsterAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (!Actor)
	{
		return;
	}
	
	if (Stimulus.WasSuccessfullySensed())
	{
		if (Actor->ActorHasTag(TEXT("Player"))||Cast<ACharacter>(Actor))
		{
			SetCurrentTarget(Actor);
			UE_LOG(LogTemp, Log, TEXT("MonsterAI: Target Acquired - %s"), *Actor->GetName());
		}
		
	}
	else
	{
		if (CurrentTarget == Actor)
		{
			ClearCurrentTarget();
			UE_LOG(LogTemp, Log, TEXT("MonsterAI: Target Lost - %s"), *Actor->GetName());
		}
	}
}
void AMonsterAIController::SetCurrentTarget(AActor* NewTarget)
{
	CurrentTarget = NewTarget;
}

void AMonsterAIController::ClearCurrentTarget()
{
	CurrentTarget = nullptr;
}