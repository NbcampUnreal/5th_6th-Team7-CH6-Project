// Fill out your copyright notice in the Description page of Project Settings.


#include "GridInventory/ItemInstance/ItemInstance.h"
#include "Net/UnrealNetwork.h"


UItemInstance::UItemInstance()
{
    InstanceID = FGuid::NewGuid();
    CurrentDurability = 0.f;

    SectionIndex = INDEX_NONE;
    PosX = 0;
    PosY = 0;

    bRotated = false;
}

void UItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UItemInstance, InstanceID);
    DOREPLIFETIME(UItemInstance, ItemID);
    DOREPLIFETIME(UItemInstance, CurrentDurability);
    DOREPLIFETIME(UItemInstance, OwnerItemGuid);
    DOREPLIFETIME(UItemInstance, RowIndex);
    DOREPLIFETIME(UItemInstance, SectionIndex);
    DOREPLIFETIME(UItemInstance, PosX);
    DOREPLIFETIME(UItemInstance, PosY);
}

void UItemInstance::InitializeIdentity(const FName& InItemID)
{
    ItemID = InItemID;
}

void UItemInstance::SetDurability(float NewDurability)
{
    CurrentDurability = FMath::Max(0.f, NewDurability);
}

void UItemInstance::AddDurability(float Delta)
{
    SetDurability(CurrentDurability + Delta);
}

void UItemInstance::SetInventoryPlacement(
    const FGuid& InOwnerItemGuid,
    int32 InRowIndex,
    int32 InSectionIndex,
    int32 InPosX,
    int32 InPosY
)
{
    OwnerItemGuid = InOwnerItemGuid;
    SectionIndex = InSectionIndex;
    RowIndex = InRowIndex;
    PosX = InPosX;
    PosY = InPosY;
}

void UItemInstance::ClearInventoryPlacement()
{
    OwnerItemGuid.Invalidate();
    SectionIndex = INDEX_NONE;
    PosX = INDEX_NONE;
    PosY = INDEX_NONE;
}

FItemInstanceSaveData UItemInstance::ToSaveData() const
{
    FItemInstanceSaveData Data;

    Data.InstanceID = InstanceID;
    Data.ItemID = ItemID;
    Data.CurrentDurability = CurrentDurability;
    Data.bRotated = bRotated;

    Data.OwnerItemGuid = OwnerItemGuid;
    Data.RowIndex = RowIndex;
    Data.SectionIndex = SectionIndex;
    Data.PosX = PosX;
    Data.PosY = PosY;

    return Data;
}

void UItemInstance::LoadFromSaveData(const FItemInstanceSaveData& Data)
{
    InstanceID = Data.InstanceID;
    ItemID = Data.ItemID;
    CurrentDurability = Data.CurrentDurability;
    bRotated = Data.bRotated;

    OwnerItemGuid = Data.OwnerItemGuid;
    RowIndex = Data.RowIndex;
    SectionIndex = Data.SectionIndex;
    PosX = Data.PosX;
    PosY = Data.PosY;
}
