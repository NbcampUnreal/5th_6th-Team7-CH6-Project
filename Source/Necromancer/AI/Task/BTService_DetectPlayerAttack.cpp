#include "BTService_DetectPlayerAttack.h"
#include "AIController.h"
#include "Necromancer.h"
#include "Component/CombatComponent.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTService_DetectPlayerAttack::UBTService_DetectPlayerAttack()
{
	NodeName = "Detect Player Attack";
	Interval = 0.1f;
	RandomDeviation = 0.02f;
}

void UBTService_DetectPlayerAttack::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB)
	{
		return;
	}

	bool bPlayerAttacking = false;

	AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(NAME_TargetActor));
	if (TargetActor)
	{
		UCombatComponent* CombatComp = TargetActor->FindComponentByClass<UCombatComponent>();
		if (CombatComp)
		{
			bPlayerAttacking = CombatComp->IsAttacking();
		}
	}

	BB->SetValueAsBool(NAME_PlayerIsAttacking, bPlayerAttacking);
}

FString UBTService_DetectPlayerAttack::GetStaticDescription() const
{
	return TEXT("Detects if target player is attacking");
}
