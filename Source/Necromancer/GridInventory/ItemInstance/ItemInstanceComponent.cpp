// Fill out your copyright notice in the Description page of Project Settings.


#include "GridInventory/ItemInstance/ItemInstanceComponent.h"
#include "GridInventory/ItemInstance/ItemInstance.h"
#include "GridInventory/GridInventoryComponent.h"
#include "GridInventory/ItemData/ItemDataSubsystem.h"

#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"

// Sets default values for this component's properties
UItemInstanceComponent::UItemInstanceComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}


int32 UItemInstanceComponent::GetCurrentDurability() const
{
	return ItemInstance->CurrentDurability;
}

void UItemInstanceComponent::GetAllItemInstances(TArray<UItemInstance*>& OutItems) const
{
	OutItems.Reset();

	// 1. 자기 자신 아이템
	if (ItemInstance)
	{
		OutItems.Add(ItemInstance);
	}

	// 2. 컨테이너라면 내부 인벤토리 아이템들
	if (InventoryComponent)
	{
		TArray<UItemInstance*> InventoryItems;
		InventoryComponent->GetInventory(InventoryItems);
		OutItems.Append(InventoryItems);
	}
}

// Called when the game starts
void UItemInstanceComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UItemInstanceComponent::Initialize(UItemInstance* InItemInstance)
{
	if (!IsValid(InItemInstance))
	{
		return;
	}

	ItemInstance = InItemInstance;
	CreateInventoryIfNeeded();
}

void UItemInstanceComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UItemInstanceComponent, ItemInstance);
}

bool  UItemInstanceComponent::ReplicateSubobjects(
	UActorChannel* Channel,
	FOutBunch* Bunch,
	FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	if (ItemInstance)
	{
		WroteSomething |= Channel->ReplicateSubobject(ItemInstance, *Bunch, *RepFlags);
	}

	return WroteSomething;
}

void UItemInstanceComponent::CreateInventoryIfNeeded()
{
	if (!ItemInstance)
	{
		return;
	}
	UDataTableSubsystem* Subsystem = GetOwner()->GetGameInstance()->GetSubsystem<UDataTableSubsystem>();
	const FItemData* Data = Subsystem->GetItemData(ItemInstance->ItemID);
	if (!Data) {
		return;
	}
	if (Data->Rows.Num() < 1) {
		return;
	}

	if (InventoryComponent)
	{
		return; // 이미 있음
	}

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	InventoryComponent = NewObject<UGridInventoryComponent>(
		Owner,
		UGridInventoryComponent::StaticClass()
	);

	InventoryComponent->RegisterComponent();	

	// 🔥 여기서 ItemInstance의 Sections 정보로 인벤토리 초기화
	// InventoryComponent->InitializeFromItemInstance(ItemInstance);
}
