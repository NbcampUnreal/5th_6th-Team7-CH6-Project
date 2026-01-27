// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterAIController.h"

#include "Necromancer.h"
#include "BehaviorTree/BlackboardComponent.h"
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




void AMonsterAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (Stimulus.WasSuccessfullySensed())
	{
		SetTargetActor(Actor);
	}
	else
	{
		GetWorldTimerManager().SetTimer(LoseTargetTimerHandle, [this]()
		{
			SetTargetActor(nullptr);
		}, 3.0f, false);
	
	}
}


