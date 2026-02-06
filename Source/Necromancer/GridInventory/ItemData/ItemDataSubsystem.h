// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GridInventory/ItemData/ItemData.h"
#include "ItemDataSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class NECROMANCER_API UItemDataSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
    UItemDataSubsystem();
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UFUNCTION(BlueprintCallable)
    bool GetItemData(FName ItemID, FItemData& OutItemData) const;

    const FItemData* GetItemData(FName ItemID) const;

private:
    UPROPERTY()
    UDataTable* ItemTable;
};
