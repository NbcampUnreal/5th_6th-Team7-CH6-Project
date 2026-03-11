#include "BTDecorator_HealthPercent.h"
#include "AIController.h"
#include "MonsterBase.h"
#include "MonsterStatComponent.h"

UBTDecorator_HealthPercent::UBTDecorator_HealthPercent()
{
	NodeName = "Health Percent";
}

bool UBTDecorator_HealthPercent::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	AAIController* AIC = OwnerComp.GetAIOwner();
	if (!AIC)
	{
		return false;
	}

	AMonsterBase* Monster = Cast<AMonsterBase>(AIC->GetPawn());
	if (!Monster)
	{
		return false;
	}

	UMonsterStatComponent* StatComp = Monster->FindComponentByClass<UMonsterStatComponent>();
	if (!StatComp)
	{
		return false;
	}

	const float MaxHP = StatComp->GetMaxHealth();
	if (MaxHP <= 0.0f)
	{
		return false;
	}

	const float HealthRatio = StatComp->GetCurrentHealth() / MaxHP;
	return HealthRatio >= MinHealthPercent && HealthRatio <= MaxHealthPercent;
}

FString UBTDecorator_HealthPercent::GetStaticDescription() const
{
	return FString::Printf(TEXT("HP: %d%% ~ %d%%"),
		FMath::RoundToInt(MinHealthPercent * 100.0f),
		FMath::RoundToInt(MaxHealthPercent * 100.0f));
}
