// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/DropItemBase.h"
#include "Character/NecPlayerCharacter.h"
#include "GridInventory/NecInventoryComponent.h"
#include "GridInventory/ItemInstance/ItemInstanceComponent.h"
#include "GridInventory/ItemInstance/ItemInstance.h"

// Sets default values
ADropItemBase::ADropItemBase()
{
    ItemInstanceComponent = CreateDefaultSubobject<UItemInstanceComponent>(TEXT("ItemInstanceComponent"));
    bReplicates = true;
}

inline void ADropItemBase::BeginPlay()
{
    Super::BeginPlay();

    if (!HasAuthority())
    {
        return;
    }

    if (!ItemInstanceComponent->GetItemInstance()) {
        UItemInstance* NewItemInstance = NewObject<UItemInstance>(this);
        ItemInstanceComponent->Initialize(NewItemInstance);
        NewItemInstance->InitializeIdentity(ItemID);
    }
}

void ADropItemBase::Interact_Implementation(AActor* Interactor)
{
    UE_LOG(LogTemp, Warning, TEXT("DropItem Interacted"));

    if (!Interactor || !ItemInstanceComponent)
    {
        return;
    }

    ANecPlayerCharacter* PlayerCharacter = Cast<ANecPlayerCharacter>(Interactor);
    if (!PlayerCharacter)
    {
        return;
    }

    // 플레이어 인벤토리 가져오기
    UNecInventoryComponent* Inventory = PlayerCharacter->GetInventoryComponent();
    if (!Inventory)
    {
        return;
    }
    if (!HasAuthority())
    {
        Server_Interact(Inventory);
    }
    else {
        Interact_Internal(Inventory);
    }
}

void ADropItemBase::Server_Interact_Implementation(UNecInventoryComponent* Inventory)
{
    Interact_Internal(Inventory);
}

void ADropItemBase::Interact_Internal(UNecInventoryComponent* Inventory)
{
    // 아이템 추가
    if (Inventory->AddItemToInventory(ItemInstanceComponent->GetItemInstance())) {
        Destroy();
    }    
}



