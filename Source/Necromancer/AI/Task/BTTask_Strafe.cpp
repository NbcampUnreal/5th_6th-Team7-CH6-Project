#include "BTTask_Strafe.h"
#include "AIController.h"
#include "Necromancer.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "NavigationSystem.h"

UBTTask_Strafe::UBTTask_Strafe()
{
	NodeName = "Strafe";
	bCreateNodeInstance = true;
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_Strafe::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIC = OwnerComp.GetAIOwner();
	if (!AIC)
	{
		return EBTNodeResult::Failed;
	}

	APawn* Pawn = AIC->GetPawn();
	if (!Pawn)
	{
		return EBTNodeResult::Failed;
	}

	ElapsedTime = 0.0f;
	StrafeDirection = FMath::RandBool() ? 1.0f : -1.0f;

	return EBTNodeResult::InProgress;
}

void UBTTask_Strafe::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	ElapsedTime += DeltaSeconds;

	if (ElapsedTime >= StrafeDuration)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	AAIController* AIC = OwnerComp.GetAIOwner();
	if (!AIC)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	APawn* Pawn = AIC->GetPawn();
	if (!Pawn)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	AActor* Target = Cast<AActor>(BB->GetValueAsObject(NAME_TargetActor));
	if (!Target)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	// 타겟→몬스터 방향에서 수직으로 이동 지점 계산
	FVector ToMonster = Pawn->GetActorLocation() - Target->GetActorLocation();
	ToMonster.Z = 0.0f;
	ToMonster.Normalize();

	// 수직 방향 (좌/우)
	FVector StrafeDir = FVector::CrossProduct(ToMonster, FVector::UpVector) * StrafeDirection;

	FVector DesiredLocation = Target->GetActorLocation() + ToMonster * StrafeRadius + StrafeDir * 200.0f;

	// NavMesh 위 유효 위치로 보정
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(Pawn->GetWorld());
	if (NavSys)
	{
		FNavLocation NavLoc;
		if (NavSys->ProjectPointToNavigation(DesiredLocation, NavLoc))
		{
			DesiredLocation = NavLoc.Location;
		}
	}

	AIC->MoveToLocation(DesiredLocation, 50.0f, false, true, false, true);
}

FString UBTTask_Strafe::GetStaticDescription() const
{
	return FString::Printf(TEXT("Strafe (%.1fs, R=%.0f)"), StrafeDuration, StrafeRadius);
}
