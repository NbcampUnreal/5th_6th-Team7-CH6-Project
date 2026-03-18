//Recovery_Item.cpp

#include "Recovery_Item.h"
#include "Component/StatComponent.h"
#include "GameFramework/Character.h"

void URecovery_Item::Use(ACharacter* User)
{
	if (!User)
	{
		return;
	}

	UStatComponent* StatComp = User->FindComponentByClass<UStatComponent>();
	if (!StatComp)
	{
		return;
	}

	StatComp->Heal(RecoverAmount);

	DecreaseDurability();
	SyncToInventory();
}