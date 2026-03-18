//Recovery_Item.cpp

#include "Recovery_Item.h"
#include "Component/StatComponent.h"
#include "Character/NecPlayerCharacter.h"
#include "GameFramework/Character.h"

void URecovery_Item::Use(ACharacter* User)
{
	if (!User)
	{
		return;
	}
	ANecPlayerCharacter* PlayerCharacter = Cast<ANecPlayerCharacter>(User);
	UStatComponent* StatComp = PlayerCharacter->GetStatComponent();
	if (!StatComp)
	{
		return;
	}

	StatComp->Heal(RecoverAmount);

	DecreaseDurability();
	SyncToInventory();
}