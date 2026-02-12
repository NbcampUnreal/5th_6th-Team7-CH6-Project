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

class UItemInstance;

UCLASS(ClassGroup = (Custom),meta = (BlueprintSpawnableComponent))
class NECROMANCER_API UNecInventoryComponent : public UGridInventoryComponent
{
	GENERATED_BODY()
public:
	UNecInventoryComponent();
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
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void AddNecInventory(AActor* NewItemActor);

	bool AddItemToInventory(UItemInstance* NewItem);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	UItemInstance* GetDefaultContainer() const;

	UFUNCTION(BlueprintCallable)
	void DropItemInWorld(TSubclassOf<AActor> SpawnActor);

	virtual void RebuildItemOwnerMap() override;
protected:
	UFUNCTION(Server, Reliable)
	void Server_AddNecInventory(AActor* NewItemActor);
	UFUNCTION(Server, Reliable)
	void Server_DropItemInWorld(TSubclassOf<AActor> SpawnActor);
private:
	void AddNecInventory_Internal(AActor* NewItemActor);
	void DropItemInWorld_Internal(TSubclassOf<AActor> SpawnActor);

	void ValidateEquipmentSlot(UItemInstance*& SlotItem);
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

	UPROPERTY(Replicated)
	AActor* HeadActor;

	UPROPERTY(Replicated)
	AActor* BodyActor;

	UPROPERTY(Replicated)
	AActor* LegsActor;

	UPROPERTY(Replicated)
	AActor* BagActor;

	UPROPERTY(Replicated)
	AActor* WeaponActor;

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
#pragma endregion


};
