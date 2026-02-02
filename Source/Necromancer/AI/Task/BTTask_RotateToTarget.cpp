// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_RotateToTarget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"
#include "Kismet/KismetMathLibrary.h"


UBTTask_RotateToTarget::UBTTask_RotateToTarget()
{
	bNotifyTick = true;
	NodeName = TEXT("RotateToTarget");
}

EBTNodeResult::Type UBTTask_RotateToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::InProgress;
	
}

void UBTTask_RotateToTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	
	AAIController* AIController = OwnerComp.GetAIOwner();
	APawn* OwnerPawn = AIController ? AIController->GetPawn() : nullptr;
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	
	if (!OwnerPawn || !Blackboard)
	{
		FinishLatentTask(OwnerComp,EBTNodeResult::Failed);
		return;
	}
	
	AActor* TargetActor = Cast<AActor>(Blackboard->GetValueAsObject("TargetActor"));
	if (!TargetActor)
	{
		FinishLatentTask(OwnerComp,EBTNodeResult::Failed);
		return;
	}
	
	FVector PawnLocation = OwnerPawn->GetActorLocation();
	FVector TargetLocation = TargetActor->GetActorLocation();
	
	FRotator PawnRotation = TargetActor->GetActorRotation();
	FRotator CurrentRotation = OwnerPawn->GetActorRotation();
	FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(PawnLocation, TargetLocation);
	
	FRotator DesiredRotation(0.0f,TargetRotation.Yaw, 0.0f);
	
	FRotator NewRotation = FMath::RInterpTo(CurrentRotation,DesiredRotation,DeltaSeconds,InterpSpeed);
	
	OwnerPawn->SetActorRotation(NewRotation);
	
	if (FMath::Abs(FMath::FindDeltaAngleDegrees(NewRotation.Yaw,DesiredRotation.Yaw)) <= FinishTolerance)
	{
		FinishLatentTask(OwnerComp,EBTNodeResult::Succeeded);
	}
}
