// Fill out your copyright notice in the Description page of Project Settings.


#include "GridInventory/ItemInstance/ItemInstance.h"
#include "Net/UnrealNetwork.h"

UItemInstance::UItemInstance()
{
    InstanceID = FGuid::NewGuid();
    ItemType = EItemType::None;
    CurrentDurability = 0.f;

    SectionIndex = INDEX_NONE;
    PosX = 0;
    PosY = 0;
}

void UItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UItemInstance, InstanceID);
    DOREPLIFETIME(UItemInstance, ItemID);
    DOREPLIFETIME(UItemInstance, ItemType);
    DOREPLIFETIME(UItemInstance, CurrentDurability);
    DOREPLIFETIME(UItemInstance, Sections);
    DOREPLIFETIME(UItemInstance, OwnerItemGuid);
    DOREPLIFETIME(UItemInstance, SectionIndex);
    DOREPLIFETIME(UItemInstance, PosX);
    DOREPLIFETIME(UItemInstance, PosY);
}
