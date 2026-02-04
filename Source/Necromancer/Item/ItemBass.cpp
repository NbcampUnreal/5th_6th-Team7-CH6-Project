// ItemBass.cpp

#include "Item/ItemBass.h"
#include "Components/SceneComponent.h"
#include "Engine/World.h"

AItemBass::AItemBass()
{
	PrimaryActorTick.bCanEverTick = false;

	USceneComponent* SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(SceneComp);

	ItemInstanceComponent = CreateDefaultSubobject<UItemInstanceComponent>(TEXT("ItemInstanceComponent"));
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
	if (Amount <= 0 || ItemData.Count <= 0)
	{
		return false;
	}

	ItemData.Count -= Amount;

	if (ItemData.Count < 0)
	{
		ItemData.Count = 0;
	}

	return true;
}

void AItemBass::OnDropped(const FVector& WorldLocation)
{
	SetActorLocation(WorldLocation);

	UE_LOG(LogTemp, Log, TEXT("Item %s dropped at location: %s"), *ItemData.ItemID.ToString(), *WorldLocation.ToString());
}
