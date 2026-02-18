// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridInventory/GridInventoryComponent.h"
#include "BucketInventoryComponent.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSubmit);
UCLASS()
class NECROMANCER_API UBucketInventoryComponent : public UGridInventoryComponent
{
	GENERATED_BODY()
public:
	UBucketInventoryComponent();

	UPROPERTY(BlueprintAssignable)
	FOnSubmit OnSubmit;
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
	UFUNCTION(BlueprintPure, Category = "Inventory")
	UItemInstance* GetDefaultContainer() const;

	virtual void RebuildItemOwnerMap()override;
private:
	UPROPERTY(Replicated)
	int32 RequirCost;
public:
	UFUNCTION(BlueprintCallable)
	void SetRequircost(int32 rcost);
	UFUNCTION(BlueprintCallable)
	int32 GetRequircost() const { return RequirCost; }
	UFUNCTION(BlueprintCallable)
	int32 GetBucketTotalCost() const;
};
