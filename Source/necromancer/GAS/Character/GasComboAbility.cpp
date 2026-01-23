#include "GAS/Character/GasComboAbility.h"

UGasComboAbility::UGasComboAbility()
{
	CurrentComboIndex = 0;
	MaxComboCount = 3;
	bNextComboInput = false;

	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

FName UGasComboAbility::GetNextSectionName() const
{
	const FString SectionStr = FString::Printf(TEXT("Section_%d"), CurrentComboIndex + 1);

	return FName(*SectionStr);
}

void UGasComboAbility::ResetCombo()
{
	CurrentComboIndex = 0;
	bNextComboInput = false;
}

void UGasComboAbility::CommitComboCost()
{
	CommitAbilityCost(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo());
}