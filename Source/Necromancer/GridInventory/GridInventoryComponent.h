// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GridInventoryComponent.generated.h"

class UItemInstance;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdated);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NECROMANCER_API UGridInventoryComponent : public UActorComponent
{
	GENERATED_BODY()
public:
    // Sets default values for this component's properties
    UGridInventoryComponent();

    UPROPERTY(BlueprintAssignable)
    FOnInventoryUpdated OnInventoryUpdated;

protected:
    // Called when the game starts
    virtual void BeginPlay() override;

    virtual void GetLifetimeReplicatedProps(
        TArray<FLifetimeProperty>& OutLifetimeProps
    ) const override;

    virtual bool ReplicateSubobjects(
        class UActorChannel* Channel,
        class FOutBunch* Bunch,
        FReplicationFlags* RepFlags
    ) override;

private:
    bool  bInventoryActive;

    UPROPERTY(Replicated)
    TArray<FGuid> RootItemGuid;
    UPROPERTY(ReplicatedUsing = OnRep_Items)
    TArray<UItemInstance*> Items;

    TMap<FGuid, TArray<UItemInstance*>> ItemsByOwnerGuid;
public:

private:
    void RebuildItemOwnerMap();    
public:
    UFUNCTION()
    void OnRep_Items();

    void SetInventory(const TArray<UItemInstance*>& InItems);

    void GetInventory(TArray<UItemInstance*>& OutItems) const;

#pragma region AddItem
public:
    bool AddItemToPos(
        UItemInstance* NewItem,
        const FGuid& ContainerGuid,
        int32 InSectionIndex,
        int32 InPosX, int32 InPosY
    );
private:
    bool CanAddItemToPos(UItemInstance* NewItem,
        const FGuid& ContainerGuid,
        int32 InSectionIndex,
        int32 InPosX, int32 InPosY
    );

    UFUNCTION(Server, Reliable)
    void Server_AddItemToPos(
        UItemInstance* NewItem,
        const FGuid& ContainerGuid,
        int32 InSectionIndex,
        int32 InPosX, int32 InPosY
    );
#pragma endregion

#pragma region RemoveItem
public:
    bool RemoveItem(UItemInstance* Item);

protected:
    UFUNCTION(Server, Reliable)
    void Server_RemoveItem(UItemInstance* Item);
#pragma endregion
		
};
