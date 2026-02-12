// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WorldActor/InteractableActor.h"
#include "SubmitBusket.generated.h"

/**
 * 
 */
class UBucketInventoryComponent;
UCLASS()
class NECROMANCER_API ASubmitBusket : public AInteractableActor
{
	GENERATED_BODY()
public:
	ASubmitBusket();
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	UBucketInventoryComponent* BucketInventory;
};
