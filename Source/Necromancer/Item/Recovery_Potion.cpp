// Recovery_Potion.cpp

#include "Item/Recovery_Potion.h"
#include "Component/StatComponent.h"
#include "GameFramework/Character.h"

ARecovery_Potion::ARecovery_Potion()
{
}

void ARecovery_Potion::ExecuteUse(ACharacter* User)
{
	if (!HasAuthority() || !User)
	{
		return;
	}

	if (UStatComponent* StatComp = User->FindComponentByClass<UStatComponent>())
	{
		StatComp->Heal(HealAmount);
	}

	Super::ExecuteUse(User);
}