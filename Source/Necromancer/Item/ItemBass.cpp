#include "ItemBass.h"
#include "GridInventory/ItemInstance/ItemInstanceComponent.h"

AItemBass::AItemBass()
{
	PrimaryActorTick.bCanEverTick = false;

	ItemInstanceComponent = CreateDefaultSubobject<UItemInstanceComponent>(TEXT("ItemInstanceComponent"));
}

void AItemBass::OnDropped(const FVector& WorldLocation)
{
	SetActorLocation(WorldLocation);
}
