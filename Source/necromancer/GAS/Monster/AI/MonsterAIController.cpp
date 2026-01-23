// Fill out your copyright notice in the Description page of Project Settings.

#include "MonsterAIController.h"
#include "GAS/Monster/MonsterBase.h"
#include "Components/StateTreeAIComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Sight.h"
#include "AbilitySystemComponent.h"
#include "MonsterGameplayTags.h"

AMonsterAIController::AMonsterAIController()
{
	PrimaryActorTick.bCanEverTick = false;

	StateTreeAIComponent = CreateDefaultSubobject<UStateTreeAIComponent>(TEXT("StateTreeAIComponent"));

	
}

void AMonsterAIController::BeginPlay()
{
	Super::BeginPlay();
	SetupPerceptionSystem();
}

void AMonsterAIController::SetupPerceptionSystem()
{
	// AAIController의 기본 PerceptionComponent 사용
	if (UAIPerceptionComponent* PerceptionComp = GetPerceptionComponent())
	{
		PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &AMonsterAIController::OnTargetPerceptionUpdated);
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
		// Player 태그가 있는 액터만 타겟으로 설정
		if (Actor->ActorHasTag(TEXT("Player")))
		{
			SetCurrentTarget(Actor);

			// Combat 태그 부여
			if (APawn* ControlledPawn = GetPawn())
			{
				if (UAbilitySystemComponent* ASC = ControlledPawn->FindComponentByClass<UAbilitySystemComponent>())
				{
					const FMonsterGameplayTags& MonsterTags = FMonsterGameplayTags::Get();
					ASC->AddLooseGameplayTag(MonsterTags.Monster_State_Combat);
				}
			}

			// 마지막 위치 클리어 (타겟이 있으니 불필요)
			ClearLastKnownLocationInternal();
		}
	}
	else
	{
		if (GetCurrentTarget() == Actor)
		{
			// 마지막 목격 위치 저장
			SetLastKnownLocation(Actor->GetActorLocation());
			ClearCurrentTarget();
		}
	}
}

void AMonsterAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (StateTreeAIComponent)
	{
		StateTreeAIComponent->StartLogic();
	}
}

void AMonsterAIController::OnUnPossess()
{
	Super::OnUnPossess();
	ClearCurrentTarget();
}

void AMonsterAIController::SetCurrentTarget(AActor* NewTarget)
{
	CurrentTarget = NewTarget;
}

void AMonsterAIController::ClearCurrentTarget()
{
	CurrentTarget = nullptr;
}

void AMonsterAIController::SetLastKnownLocation(const FVector& Location)
{
	LastKnownTargetLocation = Location;
	bHasLastKnownLocation = true;
}

void AMonsterAIController::ClearLastKnownLocationInternal()
{
	bHasLastKnownLocation = false;
	LastKnownTargetLocation = FVector::ZeroVector;
}

void AMonsterAIController::ClearLastKnownLocation()
{
	ClearLastKnownLocationInternal();

	// Combat 태그 제거
	if (APawn* ControlledPawn = GetPawn())
	{
		if (UAbilitySystemComponent* ASC = ControlledPawn->FindComponentByClass<UAbilitySystemComponent>())
		{
			const FMonsterGameplayTags& MonsterTags = FMonsterGameplayTags::Get();
			ASC->RemoveLooseGameplayTag(MonsterTags.Monster_State_Combat);
		}
	}
}

FVector AMonsterAIController::GetFocalPointOnActor(const AActor* Actor) const
{
	// Pawn의 GetActorEyesViewPoint를 사용하여 시점 결정
	if (const APawn* ControlledPawn = GetPawn())
	{
		FVector EyeLocation;
		FRotator EyeRotation;
		ControlledPawn->GetActorEyesViewPoint(EyeLocation, EyeRotation);
		return EyeLocation;
	}

	return Super::GetFocalPointOnActor(Actor);
}
