#include "BTService_AssignCombatRole.h"
#include "AIController.h"
#include "Necromancer.h"
#include "MonsterEngagementSubsystem.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTService_AssignCombatRole::UBTService_AssignCombatRole()
{
	NodeName = "Assign Combat Role";
	Interval = 0.5f;
	RandomDeviation = 0.1f;
}

void UBTService_AssignCombatRole::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
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

	UWorld* World = Pawn->GetWorld();
	if (!World)
	{
		return;
	}

	UMonsterEngagementSubsystem* Engagement = World->GetSubsystem<UMonsterEngagementSubsystem>();
	if (!Engagement)
	{
		return;
	}

	ECombatRole Role;

	if (Engagement->HasAttackSlot(Pawn, Target))
	{
		Role = ECombatRole::Attacker;
	}
	else
	{
		float Distance = FVector::Dist(Pawn->GetActorLocation(), Target->GetActorLocation());
		if (Distance <= FlankerMaxDistance)
		{
			Role = ECombatRole::Flanker;
		}
		else
		{
			Role = ECombatRole::Waiter;
		}
	}

	BB->SetValueAsEnum(NAME_CombatRole, static_cast<uint8>(Role));
}

FString UBTService_AssignCombatRole::GetStaticDescription() const
{
	return FString::Printf(TEXT("Assign Combat Role (Flanker < %.0f)"), FlankerMaxDistance);
}
