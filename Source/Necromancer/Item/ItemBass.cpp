// ItemBass.cpp

#include "Item/ItemBass.h"

AItemBass::AItemBass()
{
	PrimaryActorTick.bCanEverTick = false;
}

const FItemData* AItemBass::GetItemDataFromTable() const
{
	if (!ItemDataTable)
	{
		return nullptr;
	}

	if (ItemData.ItemID.IsNone())
	{
		return nullptr;
	}

	return ItemDataTable->FindRow<FItemData>(ItemData.ItemID,TEXT("ItemBass::GetItemDataFromTable"));
}

bool AItemBass::IsStackable() const
{
	const FItemData* Data = GetItemDataFromTable();
	if (!Data)
	{
		return false;
	}

	return Data->MaxStack > 1;
}

bool AItemBass::AddCount(int32 Amount)
{
	if (Amount <= 0)
	{
		return false;
	}

	const FItemData* Data = GetItemDataFromTable();
	if (!Data)
	{
		return false;
	}

	if (!IsStackable())
	{
		return false;
	}

	const int32 NewCount = ItemData.Count + Amount;
	ItemData.Count = FMath::Min(NewCount, Data->MaxStack);

	return true;
}

bool AItemBass::RemoveCount(int32 Amount)
{
	if (Amount <= 0)
	{
		return false;
	}

	if (ItemData.Count < Amount)
	{
		return false;
	}

	ItemData.Count -= Amount;
	return true;
}

void AItemBass::OnDropped(const FVector& WorldLocation)
{
	SetActorLocation(WorldLocation);
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
}
