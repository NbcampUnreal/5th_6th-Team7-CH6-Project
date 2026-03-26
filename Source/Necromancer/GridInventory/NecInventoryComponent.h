// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridInventory/GridInventoryComponent.h"
#include "NecInventoryComponent.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EEquipmentSlot : uint8
{
	Head    UMETA(DisplayName = "Head"),
	Body    UMETA(DisplayName = "Body"),
	Legs    UMETA(DisplayName = "Legs"),
	Bag     UMETA(DisplayName = "Bag"),
	Weapon  UMETA(DisplayName = "Weapon"),
	Default  UMETA(DisplayName = "Default")
};

UENUM()
enum class EUIState : uint8
{
	None,
	Inventory,
	Submit,
	Shop
};

class AActor;
class UItemInstance;
class UInventoryHub;
class USubmitWidgetHub;
class UBucketInventoryComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquipmentUpdated, AActor*, updateEquipment);

UCLASS(ClassGroup = (Custom),meta = (BlueprintSpawnableComponent))
class NECROMANCER_API UNecInventoryComponent : public UGridInventoryComponent
{
	GENERATED_BODY()
public:
	UNecInventoryComponent();

	UPROPERTY(BlueprintAssignable)
	FOnEquipmentUpdated OnEquipmentUpdated;
protected:
	virtual void BeginPlay() override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) 
		const override;
	virtual bool ReplicateSubobjects(
		class UActorChannel* Channel,
		class FOutBunch* Bunch,
		FReplicationFlags* RepFlags
	) override;
private:
	UPROPERTY(Replicated)
	UItemInstance* DefaultContainer;

public:
	virtual void SetInventory(const TArray<UItemInstance*>& InItems);
	virtual void LoadItemsFromSaveData(const TArray<FItemInstanceSaveData>& LoadItems);

	void LoadEquipment();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void AddNecInventory(AActor* NewItemActor);

	UFUNCTION(BlueprintCallable)
	bool AddItemToInventory(UItemInstance* NewItem);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	UItemInstance* GetDefaultContainer() const;

	UFUNCTION(BlueprintCallable)
	void DropItemInWorld(UItemInstance* DropItem);

	virtual void RebuildItemOwnerMap() override;
protected:
	UFUNCTION(Server, Reliable)
	void Server_AddNecInventory(AActor* NewItemActor);
	UFUNCTION(Server, Reliable)
	void Server_DropItemInWorld(UItemInstance* DropItem);
private:
	void AddNecInventory_Internal(AActor* NewItemActor);
	void DropItemInWorld_Internal(UItemInstance* DropItem);

	
#pragma region Equipment
private:
	UPROPERTY(Replicated)
	UItemInstance* HeadItem;

	UPROPERTY(Replicated)
	UItemInstance* BodyItem;

	UPROPERTY(Replicated)
	UItemInstance* LegsItem;

	UPROPERTY(Replicated)
	UItemInstance* BagItem;

	UPROPERTY(Replicated)
	UItemInstance* WeaponItem;

	UPROPERTY(ReplicatedUsing = OnRep_EquipmentActor)
	AActor* HeadActor;

	UPROPERTY(ReplicatedUsing = OnRep_EquipmentActor)
	AActor* BodyActor;

	UPROPERTY(ReplicatedUsing = OnRep_EquipmentActor)
	AActor* LegsActor;

	UPROPERTY(ReplicatedUsing = OnRep_EquipmentActor)
	AActor* BagActor;

	UPROPERTY(ReplicatedUsing = OnRep_EquipmentActor)
	AActor* WeaponActor;

	UFUNCTION()
	void OnRep_EquipmentActor();
public:
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	UItemInstance* GetEquipmentItem(EEquipmentSlot Slot) const;

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	AActor* GetEquipmentActor(EEquipmentSlot Slot) const;

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void EquipItem(UItemInstance* EquipItem);

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void UnequipItem(EEquipmentSlot Slot);

	void SetEquipmentItem(EEquipmentSlot Slot, UItemInstance* NewItem);
	void SetEquipmentActor(EEquipmentSlot Slot, AActor* NewActor);
protected:
	UFUNCTION(Server, Reliable)
	void Server_EquipItem(UItemInstance* EquipItem);

	void EquipItem_Internal(UItemInstance* EquipItem);

	UFUNCTION(Server, Reliable)
	void Server_UnequipItem(EEquipmentSlot Slot);

	void UnequipItem_Internal(EEquipmentSlot Slot);
private:
	void ValidateEquipmentSlot(UItemInstance*& SlotItem, AActor*& SlotActor);
#pragma endregion

#pragma region UI
private:
	EUIState CurrentUIState = EUIState::None;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UInventoryHub> InventoryWidgetClass;
	UPROPERTY()
	UInventoryHub* InventoryWidget;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<USubmitWidgetHub> SubmitWidgetClass;
	UPROPERTY()
	USubmitWidgetHub* SubmitWidget;

public:
	UFUNCTION(BlueprintCallable)
	void ToggleInventoryUI();

	UFUNCTION(BlueprintCallable)
	void ToggleSubmitUI(UBucketInventoryComponent* bucketcomponent);
	USubmitWidgetHub* GetSubmitWidget() { return SubmitWidget; }

#pragma endregion

#pragma region QuickSlot
	
	int32 SelectedQuickSlotIndex = 0;
public:
	UFUNCTION(BlueprintCallable)
	void UseItem();

	UFUNCTION(BlueprintCallable)
	int GetSelectedQuickSlotIndex()const {
		return SelectedQuickSlotIndex;
	}
protected:
	UFUNCTION(Server, Reliable)
	void Server_UseItem(int32 InSelectedIndex);
	void Internal_UseItem(int32 InSelectedIndex);

	UFUNCTION(BlueprintCallable)
	void SelectPrevQuickSlot();

	UFUNCTION(BlueprintCallable)
	void SelectNextQuickSlot();
#pragma endregion
};
