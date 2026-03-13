// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/DropItemBase.h"
#include "Character/NecPlayerCharacter.h"
#include "GridInventory/NecInventoryComponent.h"
#include "GridInventory/ItemInstance/ItemInstanceComponent.h"
#include "GridInventory/ItemInstance/ItemInstance.h"
#include "GridInventory/ItemData/ItemDataSubsystem.h"

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
    Super::Interact_Implementation(Interactor);
    UE_LOG(LogTemp, Warning, TEXT("DropItem Interacted"));

    if (!Interactor || !ItemInstanceComponent)
    {
        return;
    }

    if (!HasAuthority())
    {
        ANecPlayerCharacter* PlayerCharacter = Cast<ANecPlayerCharacter>(Interactor);
        if (!PlayerCharacter)
        {
            return;
        }
        PlayerCharacter->Server_TryInteract(this);
    }
    else {
        Interact_Internal(Interactor);
    }
}

FText ADropItemBase::GetInteractText_Implementation() const
{
    UDataTableSubsystem* Subsystem = GetWorld()->GetGameInstance()->GetSubsystem<UDataTableSubsystem>();
    if (!Subsystem)
    {
        return FText::FromString(TEXT("획득"));
    }

    if (!ItemInstanceComponent)
    {
        return FText::FromString(TEXT("획득"));
    }

    const UItemInstance* Instance = ItemInstanceComponent->GetItemInstance();
    if (!Instance)
    {
        return FText::FromString(TEXT("획득"));
    }

    const FItemData* ItemData = Subsystem->GetItemData(Instance->ItemID);
    if (!ItemData)
    {
        return FText::FromString(TEXT("획득"));
    }

    return FText::Format(
        FText::FromString(TEXT("{0} 획득")),
        ItemData->ItemName
    );
}

void ADropItemBase::Server_Interact_Implementation(AActor* Interactor)
{
    Interact_Internal(Interactor);
}

void ADropItemBase::Interact_Internal(AActor* Interactor)
{

    Super::Interact_Internal(Interactor);
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
    // 아이템 추가
    if (Inventory->AddItemToInventory(ItemInstanceComponent->GetItemInstance())) {
        TArray<UItemInstance*> TempChildItems;
        ItemInstanceComponent->GetChildItemInstances(TempChildItems);
        if (TempChildItems.Num() > 0)
        {
            Inventory->AddChildItems(TempChildItems);
        }

        HandleDestroyed();
    }    
}



