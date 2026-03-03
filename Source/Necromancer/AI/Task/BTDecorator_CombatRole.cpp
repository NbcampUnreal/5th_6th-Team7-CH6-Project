#include "BTDecorator_CombatRole.h"
#include "Necromancer.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTDecorator_CombatRole::UBTDecorator_CombatRole()
{
	NodeName = "Combat Role";
}

bool UBTDecorator_CombatRole::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB)
	{
		return false;
	}

	uint8 CurrentRole = BB->GetValueAsEnum(NAME_CombatRole);
	return CurrentRole == static_cast<uint8>(RequiredRole);
}

FString UBTDecorator_CombatRole::GetStaticDescription() const
{
	const UEnum* EnumPtr = StaticEnum<ECombatRole>();
	FString RoleName = EnumPtr ? EnumPtr->GetNameStringByValue(static_cast<int64>(RequiredRole)) : TEXT("Unknown");
	return FString::Printf(TEXT("Role == %s"), *RoleName);
}
