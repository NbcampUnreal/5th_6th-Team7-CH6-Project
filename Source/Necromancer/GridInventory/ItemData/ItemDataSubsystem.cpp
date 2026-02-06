// Fill out your copyright notice in the Description page of Project Settings.


#include "GridInventory/ItemData/ItemDataSubsystem.h"

UItemDataSubsystem::UItemDataSubsystem()
{
    static ConstructorHelpers::FObjectFinder<UDataTable> ItemTableFinder(
        TEXT("/Game/GridTestTable.GridTestTable")
    );

    if (ItemTableFinder.Succeeded())
    {
        ItemTable = ItemTableFinder.Object;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("ItemDataSubsystem: Failed to load ItemDataTable"));
    }
}

void UItemDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    
}

bool UItemDataSubsystem::GetItemData(FName ItemID, FItemData& OutItemData) const
{
    if (!ItemTable)
    {
        UE_LOG(LogTemp, Warning, TEXT("ItemDataSubsystem: ItemTable is null"));
        return false;
    }

    if (const FItemData* FoundData =
        ItemTable->FindRow<FItemData>(ItemID, TEXT("GetItemData")))
    {
        OutItemData = *FoundData; // 복사
        return true;
    }

    UE_LOG(LogTemp, Warning, TEXT("ItemDataSubsystem: ItemTable is not found item"));
    return false;
}

const FItemData* UItemDataSubsystem::GetItemData(FName ItemID) const
{
    if (!ItemTable)
    {
        UE_LOG(LogTemp, Warning, TEXT("ItemDataSubsystem: ItemTable is null"));
        return nullptr;
    }

    if (const FItemData* FoundData =
        ItemTable->FindRow<FItemData>(ItemID, TEXT("GetItemData")))
    {
        return ItemTable->FindRow<FItemData>(ItemID, TEXT("GetItemData"));
    }
    UE_LOG(LogTemp, Warning, TEXT("ItemDataSubsystem: ItemTable is not found item"));
    return nullptr;

}
