#pragma once

#include "CoreMinimal.h"
#include "GAS/Base/BaseAttributeSet.h"
#include "CharacterAttributeSet.generated.h"

UCLASS()
class NECROMANCER_API UCharacterAttributeSet : public UBaseAttributeSet
{
	GENERATED_BODY()
	
public:
	UCharacterAttributeSet();

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Stamina")
	FGameplayAttributeData Stamina;
	ATTRIBUTE_ACCESSORS(UCharacterAttributeSet, Stamina)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Stamina")
	FGameplayAttributeData MaxStamina;
	ATTRIBUTE_ACCESSORS(UCharacterAttributeSet, MaxStamina)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Attack")
	FGameplayAttributeData AttackPower;
	ATTRIBUTE_ACCESSORS(UCharacterAttributeSet, AttackPower)
};