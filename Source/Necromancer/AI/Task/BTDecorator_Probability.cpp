#include "BTDecorator_Probability.h"

UBTDecorator_Probability::UBTDecorator_Probability()
{
	NodeName = "Probability";
}

bool UBTDecorator_Probability::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	return FMath::FRand() < Probability;
}

FString UBTDecorator_Probability::GetStaticDescription() const
{
	return FString::Printf(TEXT("Probability: %d%%"), FMath::RoundToInt(Probability * 100.0f));
}
