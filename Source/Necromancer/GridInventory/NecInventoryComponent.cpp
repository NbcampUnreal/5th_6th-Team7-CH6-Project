// Fill out your copyright notice in the Description page of Project Settings.

#include "GridInventory/NecInventoryComponent.h"

#include "GridInventory/ItemData/ItemDataSubsystem.h"
#include "GridInventory/ItemInstance/ItemInstance.h"
#include "GridInventory/ItemInstance/ItemInstanceComponent.h"
#include "UI/InventoryHub.h"

#include "Engine/ActorChannel.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerState.h"

#include "Net/UnrealNetwork.h"

bool ItemTypeToEquipmentSlot(
	EItemType ItemType,
	EEquipmentSlot& OutSlot
)
{
	switch (ItemType)
	{
	case EItemType::Equipment_Head:
		OutSlot = EEquipmentSlot::Head;
		return true;

	case EItemType::Equipment_UpperBody:
		OutSlot = EEquipmentSlot::Body;
		return true;

	case EItemType::Equipment_DownBody:
		OutSlot = EEquipmentSlot::Legs;
		return true;

	case EItemType::Bag:
		OutSlot = EEquipmentSlot::Bag;
		return true;

	case EItemType::Weapon:
		OutSlot = EEquipmentSlot::Weapon;
		return true;

	default:
		return false;
	}
}

UNecInventoryComponent::UNecInventoryComponent()
{
	static ConstructorHelpers::FClassFinder<UInventoryHub> WidgetClassFinder(
		TEXT("/Game/Necromancer/Blueprints/UI/inventory/WBP_InventoryHUB"));
	if (WidgetClassFinder.Succeeded())
	{
		InventoryWidgetClass = WidgetClassFinder.Class;
	}
}

inline void UNecInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	if (GetOwner()->HasAuthority())
	{
		TArray<UItemInstance*> InItems;
		GetInventory(InItems);
		if (InItems.IsEmpty()) {
			DefaultContainer = NewObject<UItemInstance>(this);
			DefaultContainer->InitializeIdentity(
				FName("TempBag")
			);

			AddRootItem(DefaultContainer);
		}
	}
}

inline void UNecInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UNecInventoryComponent, DefaultContainer);

	// ===== Equipment : ItemInstance =====
	DOREPLIFETIME(UNecInventoryComponent, HeadItem);
	DOREPLIFETIME(UNecInventoryComponent, BodyItem);
	DOREPLIFETIME(UNecInventoryComponent, LegsItem);
	DOREPLIFETIME(UNecInventoryComponent, BagItem);
	DOREPLIFETIME(UNecInventoryComponent, WeaponItem);

	// ===== Equipment : Actor =====
	DOREPLIFETIME(UNecInventoryComponent, HeadActor);
	DOREPLIFETIME(UNecInventoryComponent, BodyActor);
	DOREPLIFETIME(UNecInventoryComponent, LegsActor);
	DOREPLIFETIME(UNecInventoryComponent, BagActor);
	DOREPLIFETIME(UNecInventoryComponent, WeaponActor);
}

bool UNecInventoryComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
	WroteSomething |= Channel->ReplicateSubobject(DefaultContainer, *Bunch, *RepFlags);
	if (HeadItem)   WroteSomething |= Channel->ReplicateSubobject(HeadItem, *Bunch, *RepFlags);
	if (BodyItem)   WroteSomething |= Channel->ReplicateSubobject(BodyItem, *Bunch, *RepFlags);
	if (LegsItem)   WroteSomething |= Channel->ReplicateSubobject(LegsItem, *Bunch, *RepFlags);
	if (BagItem)    WroteSomething |= Channel->ReplicateSubobject(BagItem, *Bunch, *RepFlags);
	if (WeaponItem) WroteSomething |= Channel->ReplicateSubobject(WeaponItem, *Bunch, *RepFlags);

	return WroteSomething;
}

void UNecInventoryComponent::SetInventory(const TArray<UItemInstance*>& InItems) {
	Super::SetInventory(InItems);
	for (UItemInstance* Item : InItems)
	{
		if (!IsValid(Item))
		{
			continue;
		}

		if (Item->OwnerItemGuid == FGuid())
		{		
			EquipItem(Item);
		}
	}
}
void UNecInventoryComponent::LoadItemsFromSaveData(const TArray<FItemInstanceSaveData>& LoadItems)
{
	Super::LoadItemsFromSaveData(LoadItems);
}
void UNecInventoryComponent::LoadEquipment()
{
	TArray<UItemInstance*> InItems;
	GetInventory(InItems);
	DefaultContainer = InItems[0];
	for (UItemInstance* Item : InItems)
	{
		if (!IsValid(Item))
		{
			continue;
		}

		if (Item->OwnerItemGuid == FGuid())
		{
			EquipItem(Item);
		}
	}
}
void UNecInventoryComponent::AddNecInventory(AActor* NewItemActor)
{
	if (!IsValid(NewItemActor))
	{
		return;
	}

	if (GetOwnerRole() < ROLE_Authority)
	{
		Server_AddNecInventory(NewItemActor);
	}
	else
	{
		AddNecInventory_Internal(NewItemActor);
	}
	return;
}

bool UNecInventoryComponent::AddItemToInventory(UItemInstance* NewItem)
{
	if (!IsValid(NewItem))
	{
		UE_LOG(LogTemp, Warning, TEXT("AddNecInventory: ItemInstance is null"));
		return false;
	}
	if (BodyItem && AddItemToContainer(NewItem, BodyItem->InstanceID))	{
	}
	else if (BagItem && AddItemToContainer(NewItem, BagItem->InstanceID))	{
	}
	else if (DefaultContainer && AddItemToContainer(NewItem, DefaultContainer->InstanceID))	{
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("AddNecInventory: Inventory is full"));
		return false;
	}
	return true;
}

UItemInstance* UNecInventoryComponent::GetDefaultContainer() const
{
	return DefaultContainer;
}

void UNecInventoryComponent::DropItemInWorld(TSubclassOf<AActor> SpawnActor)
{
	Server_DropItemInWorld(SpawnActor);
}

void UNecInventoryComponent::RebuildItemOwnerMap()
{
	Super::RebuildItemOwnerMap();
	ValidateEquipmentSlot(HeadItem, HeadActor);
	ValidateEquipmentSlot(BodyItem, BodyActor);
	ValidateEquipmentSlot(LegsItem, LegsActor);
	ValidateEquipmentSlot(BagItem, BagActor);

	UItemInstance* PrevWeaponItem = WeaponItem;
	ValidateEquipmentSlot(WeaponItem, WeaponActor);
	OnEquipmentUpdated.Broadcast(WeaponActor);
	
}

void UNecInventoryComponent::Server_AddNecInventory_Implementation(AActor* NewItemActor)
{
	AddNecInventory_Internal(NewItemActor);
}

void UNecInventoryComponent::AddNecInventory_Internal(AActor* NewItemActor)
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

	if (AddItemToInventory(NewItem)) {
		NewItemActor->Destroy();
	}
}

void UNecInventoryComponent::DropItemInWorld_Internal(TSubclassOf<AActor> SpawnActor)
{
	APlayerState* PS = Cast<APlayerState>(GetOwner());
	if (!PS) return;
	AActor* OwnerActor = Cast<AActor>(PS->GetPawn());
	if (!OwnerActor)
	{
		return;
	}

	UWorld* World = OwnerActor->GetWorld();
	if (!World)
	{
		return;
	}

	FVector Start = OwnerActor->GetActorLocation() + FVector(0.f, 0.f, 10.f);
	FVector Forward = OwnerActor->GetActorForwardVector();
	FVector End = Start + Forward * 300.f;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerActor);

	FHitResult ForwardHit;
	bool bHitForward = World->LineTraceSingleByChannel(
		ForwardHit,
		Start,
		End,
		ECC_Visibility,
		Params
	);

	FVector DownStart;
	if (bHitForward)
	{
		DownStart = ForwardHit.Location + FVector(0.f, 0.f, 50.f);
	}
	else
	{
		DownStart = End + FVector(0.f, 0.f, 50.f);
	}

	FVector DownEnd = DownStart - FVector(0.f, 0.f, 800.f);

	FHitResult DownHit;
	bool bHitDown = World->LineTraceSingleByChannel(
		DownHit,
		DownStart,
		DownEnd,
		ECC_Visibility,
		Params
	);

	if (!bHitDown)
	{
		return;
	}

	FVector SpawnLocation = DownHit.Location;
	FRotator SpawnRotation = OwnerActor->GetActorRotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = nullptr;
	SpawnParams.Instigator = nullptr;

	AActor* SpawnedItem = World->SpawnActor<AActor>(
		SpawnActor,
		SpawnLocation,
		SpawnRotation,
		SpawnParams
	);

	return;
}

inline void UNecInventoryComponent::ValidateEquipmentSlot(UItemInstance*& SlotItem, AActor*& SlotActor)
{
	bool bInvalid = false;

	if (!IsValid(SlotItem))
	{
		bInvalid = true;
	}
	else
	{
		// 장착 상태가 아니면 슬롯 비움
		if (SlotItem->OwnerItemGuid != FGuid())
		{
			bInvalid = true;
		}
		else
		{
			TArray<UItemInstance*> InventoryItems;
			GetInventory(InventoryItems);

			if (!InventoryItems.Contains(SlotItem))
			{
				bInvalid = true;
			}
		}
	}

	if (bInvalid)
	{
		SlotItem = nullptr;

		if (IsValid(SlotActor))
		{
			if (SlotActor->HasAuthority())
			{
				SlotActor->Destroy();
			}
			SlotActor = nullptr;
		}
	}
}

void UNecInventoryComponent::Server_DropItemInWorld_Implementation(TSubclassOf<AActor> SpawnActor)
{
	DropItemInWorld_Internal(SpawnActor);
}

UItemInstance* UNecInventoryComponent::GetEquipmentItem(EEquipmentSlot Slot) const
{
	switch (Slot)
	{
	case EEquipmentSlot::Head:   
		return HeadItem;
	case EEquipmentSlot::Body:   
		return BodyItem;
	case EEquipmentSlot::Legs:   
		return LegsItem;
	case EEquipmentSlot::Bag:    
		return BagItem;
	case EEquipmentSlot::Weapon: 
		return WeaponItem;
	case EEquipmentSlot::Default:	return DefaultContainer;
	default: return nullptr;
	}
}

AActor* UNecInventoryComponent::GetEquipmentActor(EEquipmentSlot Slot) const
{
	switch (Slot)
	{
	case EEquipmentSlot::Head:		return HeadActor;
	case EEquipmentSlot::Body:		return BodyActor;
	case EEquipmentSlot::Legs:		return LegsActor;
	case EEquipmentSlot::Bag:		return BagActor;
	case EEquipmentSlot::Weapon:	return WeaponActor;
	default:	return nullptr;
	}
}

void UNecInventoryComponent::EquipItem(UItemInstance* EquipItem)
{
	if (!EquipItem)
	{
		return;
	}

	if (GetOwner() && GetOwner()->HasAuthority())
	{
		EquipItem_Internal(EquipItem);
	}
	else
	{
		Server_EquipItem(EquipItem);
	}
	RebuildItemOwnerMap();
	OnInventoryUpdated.Broadcast();
}

void UNecInventoryComponent::UnequipItem(EEquipmentSlot Slot)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		UnequipItem_Internal(Slot);
	}
	else
	{
		Server_UnequipItem(Slot);
	}
	RebuildItemOwnerMap();
	OnInventoryUpdated.Broadcast();
}

void UNecInventoryComponent::SetEquipmentItem(EEquipmentSlot Slot, UItemInstance* NewItem)
{
	switch (Slot)
	{
	case EEquipmentSlot::Head:	HeadItem = NewItem;		break;
	case EEquipmentSlot::Body:	BodyItem = NewItem;		break;
	case EEquipmentSlot::Legs:	LegsItem = NewItem;		break;
	case EEquipmentSlot::Bag:	BagItem = NewItem;		break;
	case EEquipmentSlot::Weapon:WeaponItem = NewItem;	break;
	default:											break;
	}
}

void UNecInventoryComponent::SetEquipmentActor(EEquipmentSlot Slot,	AActor* NewActor)
{
	switch (Slot)
	{
	case EEquipmentSlot::Head:	HeadActor = NewActor;		break;
	case EEquipmentSlot::Body:	BodyActor = NewActor;		break;
	case EEquipmentSlot::Legs:	LegsActor = NewActor;		break;
	case EEquipmentSlot::Bag:	BagActor = NewActor;		break;
	case EEquipmentSlot::Weapon:WeaponActor = NewActor;		break;
	default:												break;
	}
}

void UNecInventoryComponent::EquipItem_Internal(UItemInstance* EquipItem)
{
	if (!EquipItem)
	{
		return;
	}

	APlayerState* PS = Cast<APlayerState>(GetOwner());
	if (!PS) return;
	AActor* OwnerActor = Cast<AActor>(PS->GetPawn());
	if (!OwnerActor)
	{
		return;
	}

	UItemDataSubsystem* Subsystem =
		GetWorld()->GetGameInstance()->GetSubsystem<UItemDataSubsystem>();

	if (!Subsystem)
	{
		return;
	}

	const FItemData* Data = Subsystem->GetItemData(EquipItem->ItemID);
	if (!Data)
	{
		return;
	}

	EEquipmentSlot TargetSlot;
	if (!ItemTypeToEquipmentSlot(Data->m_ItemType, TargetSlot))
	{
		return;
	}

	if (UItemInstance* equipItem =GetEquipmentItem(TargetSlot))
	{
		UnequipItem(TargetSlot);
	}

	AActor* SpawnedActor = nullptr;

	if (Data->EquipItemActorClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = OwnerActor;
		SpawnParams.Instigator = Cast<APawn>(OwnerActor);
		SpawnParams.SpawnCollisionHandlingOverride =
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		SpawnedActor = GetWorld()->SpawnActor<AActor>(
			Data->EquipItemActorClass,
			FTransform::Identity,
			SpawnParams
		);

		if (SpawnedActor)
		{
			SpawnedActor->SetReplicates(true);

			// 👇 캐릭터 메쉬 가져오기
			ACharacter* Character = Cast<ACharacter>(OwnerActor);
			if (Character && Character->GetMesh())
			{
				if (Data->m_ItemType == EItemType::Weapon) {
					FName SocketName = "hand_r_weapon"; // ItemData에 소켓 이름 저장해두는 게 베스트

					SpawnedActor->AttachToComponent(
						Character->GetMesh(),
						FAttachmentTransformRules::SnapToTargetNotIncludingScale,
						SocketName
					);
				}
			}
		}
	}

	SetEquipmentItem(TargetSlot, EquipItem);
	SetEquipmentActor(TargetSlot, SpawnedActor);
	AddRootItem(EquipItem);	
}

void UNecInventoryComponent::UnequipItem_Internal(EEquipmentSlot Slot)
{
	AActor* OldActor = GetEquipmentActor(Slot);
	if (OldActor)
	{
		OldActor->Destroy();
	}

	SetEquipmentActor(Slot, nullptr);
	SetEquipmentItem(Slot, nullptr);
}

void UNecInventoryComponent::ToggleInventoryUI()
{
	APlayerState* PS = Cast<APlayerState>(GetOwner());
	if (!PS) return;

	APawn* PawnOwner = PS->GetPawn();
	if (!PawnOwner) return;

	APlayerController* PC = Cast<APlayerController>(PawnOwner->GetController());
	if (!PC) return;

	// 이미 열려있으면 닫기
	if (InventoryWidget)
	{
		InventoryWidget->RemoveFromParent();
		InventoryWidget = nullptr;

		PC->bShowMouseCursor = false;
		PC->SetInputMode(FInputModeGameOnly());
		return;
	}

	// 새로 생성
	if (InventoryWidgetClass)
	{
		InventoryWidget = CreateWidget<UInventoryHub>(PC, InventoryWidgetClass);
		if (InventoryWidget)
		{
			InventoryWidget->SetInventoryComponent(this);
			InventoryWidget->AddToViewport();
			//InventoryWidget->SetInventoryComponent(this);

			PC->bShowMouseCursor = true;
			PC->SetInputMode(FInputModeGameAndUI());
		}
	}
}

void UNecInventoryComponent::Server_EquipItem_Implementation(UItemInstance* EquipItem)
{
	EquipItem_Internal(EquipItem);
}

void UNecInventoryComponent::Server_UnequipItem_Implementation(EEquipmentSlot Slot)
{
	UnequipItem_Internal(Slot);
}
