#include "BTDecorator_HasAttackSlot.h"
#include "Necromancer.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTDecorator_HasAttackSlot::UBTDecorator_HasAttackSlot()
{
	NodeName = "Has Attack Slot";
}

bool UBTDecorator_HasAttackSlot::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB)
	{
		return false;
	}

	return BB->GetValueAsBool(NAME_HasAttackSlot);
}

FString UBTDecorator_HasAttackSlot::GetStaticDescription() const
{
	return TEXT("Has Attack Slot");
}
