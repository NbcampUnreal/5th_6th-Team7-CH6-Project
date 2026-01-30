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

    bRotated = false;
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

void UItemInstance::InitializeIdentity(const FName& InItemID)
{
    //stanceID = InInstanceID;
    ItemID = InItemID;
    //emType = InItemType;
    //데이터 테이블 추가되면
    //이것저것 추가한다 꼭
}

void UItemInstance::SetDurability(float NewDurability)
{
    CurrentDurability = FMath::Max(0.f, NewDurability);
}

void UItemInstance::AddDurability(float Delta)
{
    SetDurability(CurrentDurability + Delta);
}

void UItemInstance::SetSections(const TArray<FInventorySection>& InSections)
{
    Sections = InSections;
}

void UItemInstance::SetInventoryPlacement(
    const FGuid& InOwnerItemGuid,
    int32 InSectionIndex,
    int32 InPosX,
    int32 InPosY
)
{
    OwnerItemGuid = InOwnerItemGuid;
    SectionIndex = InSectionIndex;
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
