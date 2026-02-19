// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GridInventory/ItemData/ItemData.h"
#include "ItemInstance.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnItemUpdated);

UCLASS(BlueprintType)
class NECROMANCER_API UItemInstance : public UObject
{
	GENERATED_BODY()
public:
    UItemInstance();

    UPROPERTY(BlueprintAssignable)
    FOnItemUpdated OnItemUpdated;

    virtual bool IsSupportedForNetworking() const override
    {
        return true;
    }

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
private:
    UFUNCTION()
    void OnRep_ItemChanged()
    {
        OnItemUpdated.Broadcast();
    }
public:
#pragma region Identity
    UPROPERTY(Replicated, BlueprintReadOnly)
    FGuid InstanceID;

    UPROPERTY(Replicated, BlueprintReadOnly)
    FName ItemID;

    void InitializeIdentity(
        const FName& InItemID
    );

#pragma endregion

#pragma region State
    UPROPERTY(ReplicatedUsing = OnRep_ItemChanged, BlueprintReadWrite)
    float CurrentDurability;

    UPROPERTY(ReplicatedUsing = OnRep_ItemChanged, BlueprintReadWrite)
    bool bRotated;

    void SetDurability(float NewDurability);
    void AddDurability(float Delta);

    UFUNCTION(BlueprintCallable)
    void ToggleRotation()
    {
        bRotated = !bRotated;
    }
#pragma endregion

#pragma region InventoryPlacement
    UPROPERTY(ReplicatedUsing = OnRep_ItemChanged, BlueprintReadOnly)
    FGuid OwnerItemGuid;

    UPROPERTY(ReplicatedUsing = OnRep_ItemChanged, BlueprintReadOnly)
    int32 RowIndex;

    UPROPERTY(ReplicatedUsing = OnRep_ItemChanged, BlueprintReadOnly)
    int32 SectionIndex;

    UPROPERTY(ReplicatedUsing = OnRep_ItemChanged, BlueprintReadOnly)
    int32 PosX;

    UPROPERTY(ReplicatedUsing = OnRep_ItemChanged, BlueprintReadOnly)
    int32 PosY;

    void SetInventoryPlacement(
        const FGuid& InOwnerItemGuid,
        int32 InRowIndex,
        int32 InSectionIndex,
        int32 InPosX,
        int32 InPosY
    );

    void ClearInventoryPlacement();
#pragma endregion

#pragma region Helpers
    FItemInstanceSaveData ToSaveData() const;
    void LoadFromSaveData(const FItemInstanceSaveData& Data);
#pragma endregion
};


USTRUCT(BlueprintType)
struct FItemInstanceSaveData
{
    GENERATED_BODY()

public:

    /* =========================
     * Identity
     * ========================= */

    UPROPERTY()
    FGuid InstanceID;

    UPROPERTY()
    FName ItemID;


    /* =========================
     * State
     * ========================= */

    UPROPERTY()
    float CurrentDurability = 0.f;

    UPROPERTY()
    bool bRotated = false;


    /* =========================
     * Inventory Placement
     * ========================= */

    UPROPERTY()
    FGuid OwnerItemGuid;

    UPROPERTY()
    int32 RowIndex = INDEX_NONE;

    UPROPERTY()
    int32 SectionIndex = INDEX_NONE;

    UPROPERTY()
    int32 PosX = INDEX_NONE;

    UPROPERTY()
    int32 PosY = INDEX_NONE;
};