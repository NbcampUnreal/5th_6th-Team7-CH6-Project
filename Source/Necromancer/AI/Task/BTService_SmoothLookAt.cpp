// Fill out your copyright notice in the Description page of Project Settings.

#include "BTService_SmoothLookAt.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Necromancer.h"

UBTService_SmoothLookAt::UBTService_SmoothLookAt()
{
	NodeName = "Smooth Look At Target";
	Interval = 0.0f; 
	RandomDeviation = 0.0f;
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
		return;
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
