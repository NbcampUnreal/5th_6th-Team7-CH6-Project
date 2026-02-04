//ItemBass.cpp

#include "ItemBass.h"
#include "Net/UnrealNetwork.h"
#include "GridInventory/ItemInstance/ItemInstance.h"
#include "GridInventory/ItemData/ItemData.h"

AItemBass::AItemBass()
{
	bReplicates = true;
}

void AItemBass::CopyFromItemInstance(UItemInstance* ItemInstance)
{
	if (!ItemInstance)
	{
		return;
	}

	InstanceID = ItemInstance->InstanceID;
	ItemID = ItemInstance->ItemID;
	CurrentDurability = ItemInstance->CurrentDurability;
	bRotated = ItemInstance->bRotated;
	OwnerItemGuid = ItemInstance->OwnerItemGuid;
	SectionIndex = ItemInstance->SectionIndex;
	PosX = ItemInstance->PosX;
	PosY = ItemInstance->PosY;
}

const FItemData* AItemBass::GetItemData() const
{
	if (!ItemDataTable || ItemID.IsNone())
	{
		return nullptr;
	}

	return ItemDataTable->FindRow<FItemData>(ItemID,TEXT("ItemBass::GetItemData"));
}

void AItemBass::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AItemBass, InstanceID);
	DOREPLIFETIME(AItemBass, ItemID);
	DOREPLIFETIME(AItemBass, CurrentDurability);
	DOREPLIFETIME(AItemBass, bRotated);
	DOREPLIFETIME(AItemBass, OwnerItemGuid);
	DOREPLIFETIME(AItemBass, SectionIndex);
	DOREPLIFETIME(AItemBass, PosX);
	DOREPLIFETIME(AItemBass, PosY);
}
