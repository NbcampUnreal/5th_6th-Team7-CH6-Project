// Fill out your copyright notice in the Description page of Project Settings.


#include "GridInventory/GridInventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "GridInventory/ItemInstance/ItemInstance.h"
#include "Engine/ActorChannel.h"

// Sets default values for this component's properties
UGridInventoryComponent::UGridInventoryComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = false;

    // ...
}

// Called when the game starts
void UGridInventoryComponent::BeginPlay()
{
    Super::BeginPlay();

    // ...

}

void UGridInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UGridInventoryComponent, RootItemGuid);
    DOREPLIFETIME(UGridInventoryComponent, Items);
}

bool UGridInventoryComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
    bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

    for (UItemInstance* Item : Items)
    {
        if (IsValid(Item))
        {
            WroteSomething |= Channel->ReplicateSubobject(Item, *Bunch, *RepFlags);
        }
    }

    return WroteSomething;
}

void UGridInventoryComponent::RebuildItemOwnerMap()
{
    ItemsByOwnerGuid.Reset();

    for (UItemInstance* Item : Items)
    {
        if (!Item)
            continue;

        const FGuid& OwnerGuid = Item->OwnerItemGuid;

        ItemsByOwnerGuid.FindOrAdd(OwnerGuid).Add(Item);
    }
}

inline void UGridInventoryComponent::OnRep_Items()
{
    RebuildItemOwnerMap();
}

