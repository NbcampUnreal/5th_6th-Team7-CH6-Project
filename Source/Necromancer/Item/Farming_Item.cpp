// Farming_Item.cpp

#include "Item/Farming_Item.h"

AFarming_Item::AFarming_Item()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	SetReplicateMovement(true);

	bStackable = true;
	MaxStackCount = 99;

	bCanBuyFromShop = true;
	bCanSellToShop = true;
	SellPrice = 1;
}
