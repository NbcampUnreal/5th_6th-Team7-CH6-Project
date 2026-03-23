// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GridInventoryComponent.generated.h"

class UItemInstance;
struct FItemInstanceSaveData;

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

    void RegisterItemEvents(UItemInstance* Item);

    void MarkInventoryDirty();
private:
    bool  bInventoryActive;

    UPROPERTY(Replicated)
    TArray<FGuid> RootItemGuid;
    UPROPERTY(ReplicatedUsing = OnRep_Items)
    TArray<UItemInstance*> Items;

    UPROPERTY()
    TArray<FItemInstanceSaveData> SavedItems;

    TMap<FGuid, TArray<UItemInstance*>> ItemsByOwnerGuid;

    
public:
    virtual void RebuildItemOwnerMap();    

    const TArray<FItemInstanceSaveData>& GetSavedItems() const {return SavedItems; }
    virtual void LoadItemsFromSaveData(const TArray<FItemInstanceSaveData>& LoadItems);
public:
    UFUNCTION()
    void OnRep_Items();
    UFUNCTION()
    void HandleItemChanged();

    virtual void SetInventory(const TArray<UItemInstance*>& InItems);

    void GetInventory(TArray<UItemInstance*>& OutItems) const;

    UFUNCTION(BlueprintCallable)
    void GetAllChildrenRecursive(const FGuid& ParentGuid, TArray<UItemInstance*>& OutChildren) const;

    UFUNCTION(BlueprintCallable)
    bool FindInventoryContainer(FGuid ContainerId, TArray<UItemInstance*>& OutItems);

    UFUNCTION(BlueprintCallable)
    TArray<UItemInstance*> GetItemsByNumber(int32 TargetDigit) const;

#pragma region AddItem
public:
    UFUNCTION(BlueprintCallable)
    void AddRootItem(UItemInstance* NewItem);

    UFUNCTION(BlueprintCallable)
    bool AddItemToPos(
        UItemInstance* NewItem,
        const FGuid& ContainerGuid,
        int32 InRowIndex,
        int32 InSectionIndex,
        int32 InPosX, int32 InPosY
    );

    UFUNCTION(BlueprintCallable)
    bool AddItemToContainer(
        UItemInstance* NewItem,
        const FGuid& ContainerGuid
    );

    UFUNCTION(BlueprintCallable)
    void AddChildItems(
        TArray<UItemInstance*> NewChildItems
    );
    

protected:
    UFUNCTION(BlueprintCallable)
    bool CanAddItemToPos(UItemInstance* NewItem,
        const FGuid& ContainerGuid,
        int32 InRowIndex,
        int32 InSectionIndex,
        int32 InPosX, int32 InPosY
    );
    UFUNCTION(BlueprintCallable)
    bool CanAddToConatiner(
        UItemInstance* NewItem,
        const FGuid& ContainerGuid,
        int32& OutRowIndex,
        int32& OutSectionIndex,
        int32& OutPosX,
        int32& OutPosY);

    UFUNCTION(Server, Reliable)
    void Server_AddRootItem(UItemInstance* NewItem);
    void Implement_AddRootItem(UItemInstance*& NewItem);

    UFUNCTION(Server, Reliable)
    void Server_AddItemToPos(
        UItemInstance* NewItem,
        const FGuid& ContainerGuid,
        int32 InRowIndex,
        int32 InSectionIndex,
        int32 InPosX, int32 InPosY
    );    
    void Implement_AddItemToPos(
        UItemInstance*& NewItem, 
        const FGuid& ContainerGuid,
        int32 InRowIndex,
        int32 InSectionIndex, 
        int32 InPosX, int32 InPosY);

    UFUNCTION(Server, Reliable)
    void Server_AddChildItems(
        const TArray<UItemInstance*>& NewChildItems
    );
    void Implement_AddChildItems(
        const TArray<UItemInstance*>& NewChildItems
    );
#pragma endregion

#pragma region RemoveItem
public:
    UFUNCTION(BlueprintCallable)
    bool RemoveItem(UItemInstance* Item);
    
protected:
    UFUNCTION(Server, Reliable)
    void Server_RemoveItem(UItemInstance* Item);
private:
    void Implement_RemoveItem(UItemInstance*& Item);
#pragma endregion

#pragma region OtherInventoryComponent
public:
    UFUNCTION(BlueprintCallable)
    void RequestAddItemToOther(
        UGridInventoryComponent* OtherComp,
        UItemInstance* NewItem,
        const FGuid& ContainerGuid,
        int32 InRowIndex,
        int32 InSectionIndex,
        int32 InPosX,
        int32 InPosY
    );
protected:
    UFUNCTION(Server, Reliable)
    void Server_RequestAddItemToOther(
        UGridInventoryComponent* OtherComp,
        UItemInstance* NewItem,
        FGuid ContainerGuid,
        int32 InRowIndex,
        int32 InSectionIndex,
        int32 InPosX,
        int32 InPosY
    );
private:
    void Implement_RequestAddItemToOther(
        UGridInventoryComponent* OtherComp,
        UItemInstance* NewItem,
        const FGuid& ContainerGuid,
        int32 InRowIndex,
        int32 InSectionIndex,
        int32 InPosX,
        int32 InPosY
    );
#pragma endregion
};
