// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridInventory/GridInventoryComponent.h"
#include "NecInventoryComponent.generated.h"

/**
 * 
 */
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

	UFUNCTION(BlueprintPure, Category = "Inventory")
	UItemInstance* GetDefaultContainer() const;
};
