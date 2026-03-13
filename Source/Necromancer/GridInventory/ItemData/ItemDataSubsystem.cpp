// Fill out your copyright notice in the Description page of Project Settings.


#include "GridInventory/ItemData/ItemDataSubsystem.h"
#include "Engine/DataTable.h"

UDataTableSubsystem::UDataTableSubsystem()
{
    static ConstructorHelpers::FObjectFinder<UDataTable> ItemTableFinder(TEXT("/Game/Necromancer/Blueprints/Item/DataTable/DT_ItemData.DT_ItemData")); 
    if (ItemTableFinder.Succeeded()) { ItemTable = ItemTableFinder.Object; }
    else { UE_LOG(LogTemp, Error, TEXT("ItemDataSubsystem: Failed to load ItemDataTable")); } 
    static ConstructorHelpers::FObjectFinder<UDataTable> DropTableFinder(TEXT("/Game/Necromancer/Blueprints/Item/DataTable/DT_DropInfo.DT_DropInfo")); 
    if (DropTableFinder.Succeeded()) { DropTable = DropTableFinder.Object; }
    else { UE_LOG(LogTemp, Error, TEXT("ItemDataSubsystem: Failed to load DropTable")); }
    static ConstructorHelpers::FObjectFinder<UDataTable> ActorTableFinder(TEXT("/Game/Necromancer/Blueprints/WorldActor/DT_WorldActor.DT_WorldActor"));

    if (ActorTableFinder.Succeeded()) { ActorTable = ActorTableFinder.Object; }
    else { UE_LOG(LogTemp, Error, TEXT("ItemDataSubsystem: Failed to load ActorTable")); }
}

void UDataTableSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    UE_LOG(LogTemp, Warning, TEXT("DataTableSubsystem Initialized"));

    if (!ItemTable)
    {
        UE_LOG(LogTemp, Error, TEXT("ItemTable is NULL"));
    }

    if (!DropTable)
    {
        UE_LOG(LogTemp, Error, TEXT("DropTable is NULL"));
    }

    if (!ActorTable)
    {
        UE_LOG(LogTemp, Error, TEXT("ActorTable is NULL"));
    }
    
}

bool UDataTableSubsystem::GetItemData(FName ItemID, FItemData& OutItemData) const
{
    if (!ItemTable)
    {
        UE_LOG(LogTemp, Warning, TEXT("ItemDataSubsystem: ItemTable is null"));
        return false;
    }

    const FItemData* FoundData = ItemTable->FindRow<FItemData>(ItemID, TEXT("GetItemData"));

    if (FoundData)
    {
        OutItemData = *FoundData;
        return true;
    }

    UE_LOG(LogTemp, Warning, TEXT("ItemDataSubsystem: Item not found: %s"), *ItemID.ToString());
    return false;
}

const FItemData* UDataTableSubsystem::GetItemData(FName ItemID) const
{
    if (!ItemTable)
    {
        UE_LOG(LogTemp, Warning, TEXT("ItemDataSubsystem: ItemTable is null"));
        return nullptr;
    }

    const FItemData* FoundData = ItemTable->FindRow<FItemData>(ItemID, TEXT("GetItemData"));

    if (FoundData)
    {
        return FoundData;
    }
    UE_LOG(LogTemp, Warning, TEXT("ItemDataSubsystem: ItemTable is not found item"));
    return nullptr;

}

const FworldActorInfo* UDataTableSubsystem::GetworldActorInfo(FName ItemID) const
{
    if (!ActorTable)
    {
        UE_LOG(LogTemp, Warning, TEXT("DataTableSubsystem: ActorTable is null"));
        return nullptr;
    }

    const FworldActorInfo* FoundData =
        ActorTable->FindRow<FworldActorInfo>(ItemID, TEXT("GetData"));

    if (FoundData)
    {
        return FoundData;
    }
    UE_LOG(LogTemp, Warning, TEXT("DataTableSubsystem: ActorTable is not found item"));
    return nullptr;
}

const FItemData* UDataTableSubsystem::GetRandomItemData() const
{
    if (!DropTable || !ItemTable)
    {
        UE_LOG(LogTemp, Warning, TEXT("DropTable or ItemTable is NULL"));
        return nullptr;
    }

    TArray<FDropInfo*> AllDrops;
    DropTable->GetAllRows(TEXT("DropSearch"), AllDrops);

    // 1️⃣ 레벨 필터링
    TArray<FDropInfo*> ValidDrops;

    for (FDropInfo* Drop : AllDrops)
    {
        //if (PlayerLevel >= Drop->MinLevel &&
         //   PlayerLevel <= Drop->MaxLevel)
        {
            ValidDrops.Add(Drop);
        }
    }

    if (ValidDrops.Num() == 0)
        return nullptr;

    // 2️⃣ 총 가중치 계산
    int32 TotalWeight = 0;
    for (const FDropInfo* Drop : ValidDrops)
    {
        TotalWeight += Drop->DropWeight;
    }

    if (TotalWeight <= 0)
    {
        UE_LOG(LogTemp, Error, TEXT("Total DropWeight is ZERO"));
        return nullptr;
    }

    // 3️⃣ 랜덤 값 생성
    int32 RandomValue = FMath::RandRange(0, TotalWeight - 1);

    // 4️⃣ 누적합 비교
    int32 AccWeight = 0;
    for (const FDropInfo* Drop : ValidDrops)
    {
        AccWeight += Drop->DropWeight;

        if (RandomValue < AccWeight)
        {
            UE_LOG(LogTemp, Log, TEXT("Drop Selected: %s"), *Drop->ItemID.ToString());

            const FItemData* ItemData =
                ItemTable->FindRow<FItemData>(Drop->ItemID, TEXT("FindItemData"));

            if (!ItemData)
            {
                UE_LOG(LogTemp, Error, TEXT("ItemData NOT FOUND: %s"), *Drop->ItemID.ToString());
                return nullptr;
            }
            UE_LOG(LogTemp, Warning, TEXT("Item FOUND: %s"), *ItemData->ItemName.ToString());
            return ItemData;
        }
    }
    return nullptr;
}
