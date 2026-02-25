// Fill out your copyright notice in the Description page of Project Settings.

#include "BTService_SmoothLookAt.h"
#include "AIController.h"
#include "MonsterBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Necromancer.h"

UBTService_SmoothLookAt::UBTService_SmoothLookAt()
{
	NodeName = "Smooth Look At Target";
	Interval = 0.0f;
	RandomDeviation = 0.0f;
	bNotifyCeaseRelevant = true;
}

void UBTService_SmoothLookAt::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIC = OwnerComp.GetAIOwner();
	if (!AIC)
	{
		return;
	}

	APawn* Pawn = AIC->GetPawn();
	if (!Pawn)
	{
		return;
	}

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB)
	{
		return;
	}

	AActor* Target = Cast<AActor>(BB->GetValueAsObject(NAME_TargetActor));
	if (!Target)
	{
		// 타겟 없으면 순찰 모드로 전환
		if (AMonsterBase* Monster = Cast<AMonsterBase>(Pawn))
		{
			Monster->SetCombatMovementMode(false);
		}
		return;
	}

	// 타겟 있으면 전투 모드 (이동 방향 회전 OFF → Strafe 가능)
	if (AMonsterBase* Monster = Cast<AMonsterBase>(Pawn))
	{
		Monster->SetCombatMovementMode(true);
	}

	FVector PawnLocation = Pawn->GetActorLocation();
	FVector TargetLocation = Target->GetActorLocation();

	FRotator CurrentRotation = Pawn->GetActorRotation();
	FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(PawnLocation, TargetLocation);

	if (bYawOnly)
	{
		TargetRotation.Pitch = CurrentRotation.Pitch;
		TargetRotation.Roll = CurrentRotation.Roll;
	}

	FRotator NewRotation = FMath::RInterpConstantTo(
		CurrentRotation,
		TargetRotation,
		DeltaSeconds,
		RotationSpeed
	);

	Pawn->SetActorRotation(NewRotation);
}

void UBTService_SmoothLookAt::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnCeaseRelevant(OwnerComp, NodeMemory);

	// 서비스가 비활성화되면 순찰 모드로 복귀
	if (AAIController* AIC = OwnerComp.GetAIOwner())
	{
		if (AMonsterBase* Monster = Cast<AMonsterBase>(AIC->GetPawn()))
		{
			Monster->SetCombatMovementMode(false);
		}
	}
}
