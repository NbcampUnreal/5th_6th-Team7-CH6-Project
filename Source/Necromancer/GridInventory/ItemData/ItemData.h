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
    int32 MaxStack; //인벤토리 한칸의 크기

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
    int32 MaxDurability;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
    int32 Weight;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
    int32 Width;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
    int32 Height;
};
