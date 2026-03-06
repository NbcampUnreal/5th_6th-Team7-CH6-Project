// Fill out your copyright notice in the Description page of Project Settings.


#include "GridInventory/BucketInventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "GridInventory/ItemInstance/ItemInstance.h"
#include "GridInventory/ItemInstance/ItemInstanceComponent.h"
#include "GridInventory/ItemData/ItemDataSubsystem.h"
#include "Engine/ActorChannel.h"

UBucketInventoryComponent::UBucketInventoryComponent()
{
}

void UBucketInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	if (GetOwner()->HasAuthority())
	{
		DefaultContainer = NewObject<UItemInstance>(this);
		DefaultContainer->InitializeIdentity(
			FName("BucketBag")
		);

		AddRootItem(DefaultContainer);
	}
}

void UBucketInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UBucketInventoryComponent, DefaultContainer);
}

bool UBucketInventoryComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
	WroteSomething |= Channel->ReplicateSubobject(DefaultContainer, *Bunch, *RepFlags);
    return WroteSomething;
}

UItemInstance* UBucketInventoryComponent::GetDefaultContainer() const
{
	return DefaultContainer;
}

void UBucketInventoryComponent::RebuildItemOwnerMap()
{
    Super::RebuildItemOwnerMap();
    if (RequirCost < GetBucketTotalCost()) {
        OnInventoryUpdated.Broadcast();
        OnSubmit.Broadcast();
    }
}

void UBucketInventoryComponent::SetRequircost(int32 rcost)
{
    RequirCost = rcost;
}

int32 UBucketInventoryComponent::GetBucketTotalCost() const
{
    if (!DefaultContainer)
        return 0;

    UDataTableSubsystem* Subsystem =
        GetWorld()->GetGameInstance()->GetSubsystem<UDataTableSubsystem>();

    if (!Subsystem)
        return 0;

    int32 TotalCost = 0;

    TArray<UItemInstance*> InventoryItems;
    GetInventory(InventoryItems);

    for (UItemInstance* Item : InventoryItems)
    {
        if (!IsValid(Item))
            continue;
        if (Item->OwnerItemGuid == DefaultContainer->InstanceID)
        {
            const FItemData* Data = Subsystem->GetItemData(Item->ItemID);
            if (Data)
            {
                TotalCost += Data->Cost;
            }
        }
    }

    return TotalCost;
}
