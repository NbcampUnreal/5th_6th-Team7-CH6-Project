// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryHub.generated.h"

/**
 * 
 */
class UNecInventoryComponent;

UCLASS()
class NECROMANCER_API UInventoryHub : public UUserWidget
{
	GENERATED_BODY()
public:

    UFUNCTION(BlueprintCallable)
    void SetInventoryComponent(UNecInventoryComponent* InInventoryComponent);

    UFUNCTION(BlueprintCallable)
    UNecInventoryComponent* GetInventoryComponent() const;
protected:

    UPROPERTY(BlueprintReadOnly, Category = "Inventory")
    UNecInventoryComponent* InventoryComponent;
};
