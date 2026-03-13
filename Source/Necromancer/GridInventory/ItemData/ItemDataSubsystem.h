// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/DataTable.h"
#include "GridInventory/ItemData/ItemData.h"
#include "ItemDataSubsystem.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class NECROMANCER_API UDataTableSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
    UDataTableSubsystem();
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UFUNCTION(BlueprintCallable)
    bool GetItemData(FName ItemID, FItemData& OutItemData) const;

    UFUNCTION(BlueprintCallable)
    bool IsItemContainer(FName ItemID) const;

    const FItemData* GetItemData(FName ItemID) const;

    const FworldActorInfo* GetworldActorInfo(FName ItemID) const;

    const FItemData* GetRandomItemData() const;

private:
    UPROPERTY(EditDefaultsOnly, Category = "DataTable")
    TSoftObjectPtr<UDataTable> ItemTable;

    UPROPERTY(EditDefaultsOnly, Category = "DataTable")
    TSoftObjectPtr<UDataTable> DropTable;

    UPROPERTY(EditDefaultsOnly, Category = "DataTable")
    TSoftObjectPtr<UDataTable> ActorTable;
};
