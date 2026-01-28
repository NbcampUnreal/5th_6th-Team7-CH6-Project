// ItemBass.cpp

#include "Item/ItemBass.h"

AItemBass::AItemBass()
{
	PrimaryActorTick.bCanEverTick = false;
}

bool AItemBass::AddCount(int32 Amount)
{
	if (Amount <= 0)
	{
		return false;
	}

	ItemData.Count += Amount;
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
