// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldActor/SubmitBusket.h"
#include "GridInventory/BucketInventoryComponent.h"

ASubmitBusket::ASubmitBusket()
{
    PrimaryActorTick.bCanEverTick = false;

    BucketInventory = CreateDefaultSubobject<UBucketInventoryComponent>(TEXT("BucketInventory"));
    bReplicates = true;
}