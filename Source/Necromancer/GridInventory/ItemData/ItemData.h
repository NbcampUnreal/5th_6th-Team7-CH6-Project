// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemData.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EItemType : uint8
{
    None,//0
    Bag,//1
    Equipment_Head,//2
    Equipment_UpperBody,//3
    Equipment_DownBody,//4
    Consumable,//5
    Weapon //6
};

USTRUCT(BlueprintType)
struct FInventorySection
{
    GENERATED_BODY()

public:
    FInventorySection()
        : Width(0)
        , Height(0)
    {
    }

    FInventorySection(int32 InWidth, int32 InHeight)
        : Width(InWidth)
        , Height(InHeight)
    {
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Width;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Height;
};

USTRUCT(BlueprintType)
struct FInventoryRow
{
    GENERATED_BODY()

public:
    FInventoryRow() {}

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FInventorySection> Sections;
};

USTRUCT(BlueprintType)
struct NECROMANCER_API FItemData : public FTableRowBase
{
	GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
    FName ItemID; // 아이템ID (아이템 타)

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
    FText ItemName; // 유저가 보는 아이템 이름 사과, 체력포션

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
    EItemType m_ItemType;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
    UTexture2D* Thumbnail; // 인벤토리에서 보여줄 아이템 썸네일

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
    TSubclassOf<AActor> DropItemActorClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
    TSubclassOf<AActor> EquipItemActorClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
    TSubclassOf<UObject> UseActionClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
    TArray<FInventoryRow> Rows;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
    int32 MaxStack; //인벤토리 한칸의 크기

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
    int32 MaxDurability;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
    int32 Weight;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
    int32 Width;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
    int32 Height;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
    int32 Cost;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 SpawnCost;
};


USTRUCT(BlueprintType)
struct FDropInfo : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    FName ItemID;

    UPROPERTY(EditAnywhere)
    int32 DropWeight;

    UPROPERTY(EditAnywhere)
    int32 MinLevel;

    UPROPERTY(EditAnywhere)
    int32 MaxLevel;

    UPROPERTY(EditAnywhere)
    TArray<FName> Tags;
};


USTRUCT(BlueprintType)
struct FworldActorInfo : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    FName ActorID;

    UPROPERTY(EditAnywhere)
    TSubclassOf<AActor> WorldActorClass;

    UPROPERTY(EditAnywhere)
    int32 Level;
};

USTRUCT(BlueprintType)
struct FDropEntry : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    FDataTableRowHandle ItemRow;

    UPROPERTY(EditAnywhere)
    int32 Weight = 1;

    int32 GetItemCost() const
    {
        if (ItemRow.DataTable)
        {
            if (FItemData* Row = ItemRow.GetRow<FItemData>("DropEntry"))
            {
                return Row->Cost;
            }
        }
        return 0;
    }
};

class FDropManager
{
public:

    int32 CurrentCost = 0;
    int32 MaxCost = 100;

    UDataTable* ItemDataTable = nullptr;

    bool TrySpawnRandomItem(UWorld* World, FVector SpawnLocation)
    {
        if (!World || !ItemDataTable)
            return false;

        TArray<FName> RowNames = ItemDataTable->GetRowNames();

        TArray<FItemData*> ValidItems;

        for (const FName& RowName : RowNames)
        {
            FItemData* Item = ItemDataTable->FindRow<FItemData>(RowName, "DropCheck");
            if (!Item)
                continue;
            if (CurrentCost + Item->Cost <= MaxCost)
            {
                ValidItems.Add(Item);
            }
        }

        if (ValidItems.Num() == 0)
            return false;

        int32 RandomIndex = FMath::RandRange(0, ValidItems.Num() - 1);
        FItemData* SelectedItem = ValidItems[RandomIndex];

        if (!SelectedItem || !SelectedItem->DropItemActorClass)
            return false;

        World->SpawnActor<AActor>(SelectedItem->DropItemActorClass,SpawnLocation,FRotator::ZeroRotator);

        CurrentCost += SelectedItem->Cost;

        return true;
    }
};

USTRUCT(BlueprintType)
struct FWorldActorSpawnData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    FName ItemID;

    UPROPERTY(EditAnywhere)
    USceneComponent* SpawnPoint;
};