// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GridInventory/ItemData/ItemData.h"
#include "ItemInstance.generated.h"

/**
 * 
 */
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

UCLASS()
class NECROMANCER_API UItemInstance : public UObject
{
	GENERATED_BODY()
public:
    UItemInstance();

    virtual bool IsSupportedForNetworking() const override
    {
        return true;
    }

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

#pragma region Identity
    UPROPERTY(Replicated, BlueprintReadOnly)
    FGuid InstanceID;

    UPROPERTY(Replicated, BlueprintReadOnly)
    FName ItemID;

    UPROPERTY(Replicated, BlueprintReadOnly)
    EItemType ItemType;
#pragma endregion

#pragma region State
    UPROPERTY(Replicated, BlueprintReadWrite)
    float CurrentDurability;

    UPROPERTY(Replicated, BlueprintReadWrite)
    bool bRotated;
#pragma endregion

#pragma region Container
    UPROPERTY(Replicated, BlueprintReadOnly)
    TArray<FInventorySection> Sections;
#pragma endregion

#pragma region InventoryPlacement
    UPROPERTY(Replicated, BlueprintReadOnly)
    FGuid OwnerItemGuid;

    UPROPERTY(Replicated, BlueprintReadOnly)
    int32 SectionIndex;

    UPROPERTY(Replicated, BlueprintReadOnly)
    int32 PosX;

    UPROPERTY(Replicated, BlueprintReadOnly)
    int32 PosY;
#pragma endregion

#pragma region Helpers
    UFUNCTION(BlueprintCallable)
    bool IsContainer() const
    {
        return Sections.Num() > 0;
    }
#pragma endregion
};
