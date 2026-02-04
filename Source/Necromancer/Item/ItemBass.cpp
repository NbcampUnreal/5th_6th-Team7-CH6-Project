// ItemBass.cpp

#include "Item/ItemBass.h"

#include "GridInventory/ItemInstance/ItemInstance.h"
#include "GridInventory/ItemInstance/ItemInstanceComponent.h"
#include "Net/UnrealNetwork.h"

AItemBass::AItemBass()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	USceneComponent* RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(RootScene);

	ItemInstanceComponent = CreateDefaultSubobject<UItemInstanceComponent>(TEXT("ItemInstanceComponent"));
}

void AItemBass::InitializeWithItemInstance(UItemInstance* InItemInstance)
{
	if (!InItemInstance || !ItemInstanceComponent)
	{
		return;
	}

	if (HasAuthority())
	{
		ItemInstanceComponent->Initialize(InItemInstance);
	}
}

FGuid AItemBass::GetInstanceID() const
{
	if (!ItemInstanceComponent || !ItemInstanceComponent->GetItemInstance())
	{
		return FGuid();
	}

	return ItemInstanceComponent->GetItemInstance()->InstanceID;
}

FName AItemBass::GetItemID() const
{
	if (!ItemInstanceComponent || !ItemInstanceComponent->GetItemInstance())
	{
		return NAME_None;
	}

	return ItemInstanceComponent->GetItemInstance()->ItemID;
}

float AItemBass::GetDurability() const
{
	if (!ItemInstanceComponent || !ItemInstanceComponent->GetItemInstance())
	{
		return 0.f;
	}

	return ItemInstanceComponent->GetItemInstance()->CurrentDurability;
}

void AItemBass::SetDurability(float NewDurability)
{
	if (!ItemInstanceComponent || !ItemInstanceComponent->GetItemInstance())
	{
		return;
	}

	if (HasAuthority())
	{
		ItemInstanceComponent->GetItemInstance()->SetDurability(NewDurability);
	}
}
