// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/InventoryHub.h"
#include "SubmitWidgetHub.generated.h"

/**
 * 
 */
class UBucketInventoryComponent;
UCLASS()
class NECROMANCER_API USubmitWidgetHub : public UInventoryHub
{
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintCallable)
    void SetBucketInventoryComponent(UBucketInventoryComponent* InBucketInventoryComponent);

    UFUNCTION(BlueprintCallable)
    UBucketInventoryComponent* GetBucketInventoryComponent() const;

protected:

    UPROPERTY(BlueprintReadOnly, Category = "Inventory")
    UBucketInventoryComponent* BucketInventoryComponent;
};