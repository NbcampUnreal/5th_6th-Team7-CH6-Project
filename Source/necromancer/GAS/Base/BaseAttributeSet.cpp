#include "GAS/Base/BaseAttributeSet.h"
#include "GameplayEffectExtension.h"

UBaseAttributeSet::UBaseAttributeSet()
{
	InitMaxHealth(100.0f);
	InitHealth(100.0f);
	InitDefense(5.0f);
	InitMoveSpeed(300.0f);
}

void UBaseAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
}

void UBaseAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		const float Damage = GetIncomingDamage();
		SetIncomingDamage(0.0f);

		if (Damage > 0.0f)
		{
			// 데미지 = 원본데미지 * (100 / (100 + 방어력))
			const float DefenseValue = GetDefense();
			const float FinalDamage = Damage * (100.0f / (100.0f + DefenseValue));

			const float NewHealth = GetHealth() - FinalDamage;
			SetHealth(FMath::Clamp(NewHealth, 0.0f, GetMaxHealth()));

			// TODO: 사망 처리
		}
	}
}
