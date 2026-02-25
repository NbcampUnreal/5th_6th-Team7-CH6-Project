// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItemInstanceComponent.generated.h"


class UItemInstance;
class UGridInventoryComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class NECROMANCER_API UItemInstanceComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UItemInstanceComponent();

	/** 아이템 인스턴스 초기화 */
	UFUNCTION(BlueprintCallable, Category = "Item")
	void Initialize(UItemInstance* InItemInstance);

	/** 컨테이너인지 여부 */
	bool HasInventory() const { return InventoryComponent != nullptr; }

	UGridInventoryComponent* GetInventory() const { return InventoryComponent; }
	UItemInstance* GetItemInstance() const { return ItemInstance; }
	int32 GetCurrentDurability() const;
	void GetAllItemInstances(TArray<UItemInstance*>& OutItems) const;

protected:
	virtual void BeginPlay() override;

private:
	/** 이 컴포넌트가 관리하는 아이템 인스턴스 */
	UPROPERTY()
	UItemInstance* ItemInstance = nullptr;

	/** 컨테이너일 경우에만 생성됨 */
	UPROPERTY()
	UGridInventoryComponent* InventoryComponent = nullptr;

	void CreateInventoryIfNeeded();
};
