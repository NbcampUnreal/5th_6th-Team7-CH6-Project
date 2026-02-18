// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SubmitWidgetHub.h"
#include "GridInventory/BucketInventoryComponent.h"

void USubmitWidgetHub::SetBucketInventoryComponent(UBucketInventoryComponent* InBucketInventoryComponent)
{
	BucketInventoryComponent = InBucketInventoryComponent;
}

UBucketInventoryComponent* USubmitWidgetHub::GetBucketInventoryComponent() const
{
	return BucketInventoryComponent;
}
