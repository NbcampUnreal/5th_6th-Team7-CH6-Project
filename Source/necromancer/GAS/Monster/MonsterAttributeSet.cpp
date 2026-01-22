#include "GAS/Monster/MonsterAttributeSet.h"
#include "GAS/Monster/Interface/MonsterCombatInterface.h"
#include "GameplayEffectExtension.h"

UMonsterAttributeSet::UMonsterAttributeSet()
{
	InitAttackPower(10.0f);
	InitAttackRange(150.0f);
	InitDetectionRange(500.0f);
}

void UMonsterAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	// 부모 클래스에서 데미지 계산 및 Health 적용
	Super::PostGameplayEffectExecute(Data);

	// 몬스터 전용 사망 처리
	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		if (GetHealth() <= 0.0f)
		{
			AActor* OwnerActor = GetOwningActor();
			if (OwnerActor && OwnerActor->Implements<UMonsterCombatInterface>())
			{
				IMonsterCombatInterface::Execute_HandleDeath(OwnerActor);
			}
		}
	}
}