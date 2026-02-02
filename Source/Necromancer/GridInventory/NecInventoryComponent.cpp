// Fill out your copyright notice in the Description page of Project Settings.


#include "GridInventory/NecInventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "GridInventory/ItemInstance/ItemInstance.h"
#include "GridInventory/ItemInstance/ItemInstanceComponent.h"
#include "Engine/ActorChannel.h"

UNecInventoryComponent::UNecInventoryComponent()
{
}

inline void UNecInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	if (GetOwner()->HasAuthority())
	{
		DefaultContainer = NewObject<UItemInstance>(this);
		DefaultContainer->InitializeIdentity(
			FName("TempBag")
		);

		AddRootItem(DefaultContainer);
	}
}

inline void UNecInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UNecInventoryComponent, DefaultContainer);
}

bool UNecInventoryComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
	WroteSomething |= Channel->ReplicateSubobject(DefaultContainer, *Bunch, *RepFlags);

	return WroteSomething;
}

void UNecInventoryComponent::AddNecInventory(AActor* NewItemActor)
{
	if (!IsValid(NewItemActor))
	{
		return;
	}

	
	UItemInstanceComponent* ItemComp =
		NewItemActor->FindComponentByClass<UItemInstanceComponent>();

	if (!IsValid(ItemComp))
	{
		return;
	}

	UItemInstance* NewItem = ItemComp->GetItemInstance();

	if (!IsValid(NewItem))
	{
		UE_LOG(LogTemp, Warning, TEXT("AddNecInventory: ItemInstance is null"));
		return;
	}
	if (AddItemToContainer(NewItem,
		DefaultContainer->InstanceID)) {
		NewItemActor->Destroy();
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("AddNecInventory: Inventory is full"));
		return;
	}
}

