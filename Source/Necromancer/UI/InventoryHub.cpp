// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InventoryHub.h"
#include "GridInventory/NecInventoryComponent.h"

void UInventoryHub::SetInventoryComponent(UNecInventoryComponent* InInventoryComponent)
{
	InventoryComponent = InInventoryComponent;
}

UNecInventoryComponent* UInventoryHub::GetInventoryComponent() const
{
	return InventoryComponent;
}