#pragma once

#include "CoreMinimal.h"
#include "GAS/Base/BaseAttributeSet.h"
#include "MonsterAttributeSet.generated.h"

UCLASS()
class NECROMANCER_API UMonsterAttributeSet : public UBaseAttributeSet
{
	GENERATED_BODY()

public:
	UMonsterAttributeSet();
	
	// 사망 처리를 위해 오버라이드
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	FGameplayAttributeData AttackPower;
	ATTRIBUTE_ACCESSORS(UMonsterAttributeSet, AttackPower)

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	FGameplayAttributeData AttackRange;
	ATTRIBUTE_ACCESSORS(UMonsterAttributeSet, AttackRange)

	UPROPERTY(BlueprintReadOnly, Category = "AI")
	FGameplayAttributeData DetectionRange;
	ATTRIBUTE_ACCESSORS(UMonsterAttributeSet, DetectionRange)
};
