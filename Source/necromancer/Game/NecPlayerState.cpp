#include "Game/NecPlayerState.h"
#include "GAS/Character/CharacterAttributeSet.h"
#include "AbilitySystemComponent.h"

ANecPlayerState::ANecPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed); // Temp

	AttributeSet = CreateDefaultSubobject<UCharacterAttributeSet>(TEXT("AttributeSet"));
}

UAbilitySystemComponent* ANecPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}