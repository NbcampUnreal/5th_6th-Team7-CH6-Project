// Fill out your copyright notice in the Description page of Project Settings.


#include "GridInventory/GridInventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "GridInventory/ItemInstance/ItemInstance.h"
#include "Engine/ActorChannel.h"

// Sets default values for this component's properties
UGridInventoryComponent::UGridInventoryComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = false;

    // ...
}

// Called when the game starts
void UGridInventoryComponent::BeginPlay()
{
    Super::BeginPlay();
    if (GetOwner()->HasAuthority())
    {
        // 서버는 항상 활성
        bInventoryActive = true;
        RebuildItemOwnerMap();
        return;
    }

    APawn* OwnerPawn = Cast<APawn>(GetOwner());
    if (!OwnerPawn)
    {
        // Pawn이 아니면 기본적으로 비활성 (상자 등)
        bInventoryActive = false;
        return;
    }

    APlayerController* PC = Cast<APlayerController>(OwnerPawn->GetController());
    if (PC && PC->IsLocalController())
    {
        bInventoryActive = true;
        RebuildItemOwnerMap();
    }
    else
    {
        bInventoryActive = false;
    }

}

void UGridInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UGridInventoryComponent, RootItemGuid);
    DOREPLIFETIME(UGridInventoryComponent, Items);
}

bool UGridInventoryComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
    bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

    for (UItemInstance* Item : Items)
    {
        if (IsValid(Item))
        {
            WroteSomething |= Channel->ReplicateSubobject(Item, *Bunch, *RepFlags);
        }
    }

    return WroteSomething;
}

void UGridInventoryComponent::RebuildItemOwnerMap()
{
    ItemsByOwnerGuid.Reset();

    for (UItemInstance* Item : Items)
    {
        if (!Item)
            continue;

        const FGuid& OwnerGuid = Item->OwnerItemGuid;

        ItemsByOwnerGuid.FindOrAdd(OwnerGuid).Add(Item);
    }
}

inline void UGridInventoryComponent::OnRep_Items()
{
    if (bInventoryActive)
    {
        RebuildItemOwnerMap();
    }
}

inline void UGridInventoryComponent::SetInventory(const TArray<UItemInstance*>& InItems) {
    Items = InItems;
    RebuildItemOwnerMap();

    /*void AMyCharacter::PossessedBy(AController * NewController)
    {
        Super::PossessedBy(NewController);

        if (HasAuthority())
        {
            if (AMyPlayerState* PS = GetPlayerState<AMyPlayerState>())
            {
                if (UGridInventoryComponent* Inv =
                    FindComponentByClass<UGridInventoryComponent>())
                {
                    Inv->SetInventory(PS->Items);
                }
            }
        }
    }*/
}

inline void UGridInventoryComponent::GetInventory(TArray<UItemInstance*>& OutItems) const
{
    OutItems = Items;
}

bool UGridInventoryComponent::AddItemToPos(UItemInstance* NewItem, 
    const FGuid& ContainerGuid, 
    int32 InSectionIndex, 
    int32 InPosX, int32 InPosY)
{
    if (!IsValid(NewItem))
    {
        return false;
    }

    if (!CanAddItemToPos(
        NewItem,
        ContainerGuid,
        InSectionIndex,
        InPosX,
        InPosY))
    {
        return false;
    }

    if (GetOwnerRole() < ROLE_Authority)
    {
        Server_AddItemToPos(
            NewItem,
            ContainerGuid,
            InSectionIndex,
            InPosX,
            InPosY
        );
    }
    else
    {
        Server_AddItemToPos(
            NewItem,
            ContainerGuid,
            InSectionIndex,
            InPosX,
            InPosY
        );
    }
    return true;
}

bool UGridInventoryComponent::CanAddItemToPos(UItemInstance* NewItem, 
    const FGuid& ContainerGuid,
    int32 InSectionIndex,
    int32 InPosX, int32 InPosY)
{
    if (!IsValid(NewItem))
    {
        return false;
    }

    /* 1. 컨테이너 아이템 찾기 */
    UItemInstance* ContainerItem = nullptr;
    for (UItemInstance* Item : Items)
    {
        if (IsValid(Item) && Item->InstanceID == ContainerGuid)
        {
            ContainerItem = Item;
            break;
        }
    }

    if (!IsValid(ContainerItem))
    {
        return false;
    }

    /* 2. 컨테이너 여부 */
    if (!ContainerItem->IsContainer())
    {
        return false;
    }

    /* 3. 섹션 인덱스 체크 */
    if (!ContainerItem->Sections.IsValidIndex(InSectionIndex))
    {
        return false;
    }

    const FInventorySection& Section =
        ContainerItem->Sections[InSectionIndex];

    /* 4. 아이템 크기 계산 (회전 고려) */
    int32 ItemWidth = NewItem->bRotated ? /*Height*/ 1 : /*Width*/ 1;
    int32 ItemHeight = NewItem->bRotated ? /*Width*/ 1 : /*Height*/ 1;
    // ↑ 여기 반드시 ItemData 기반으로 교체해줘야 함

    /* 5. 섹션 범위 체크 */
    if (InPosX < 0 || InPosY < 0)
    {
        return false;
    }

    if (InPosX + ItemWidth > Section.Width ||
        InPosY + ItemHeight > Section.Height)
    {
        return false;
    }

    /* 6. 같은 컨테이너 내부 아이템 충돌 검사 */
    TArray<UItemInstance*>* ItemsInContainer =
        ItemsByOwnerGuid.Find(ContainerGuid);

    if (!ItemsInContainer)
    {
        // 아직 아이템이 하나도 없는 컨테이너
        return true;
    }

    for (UItemInstance* ExistingItem : *ItemsInContainer)
    {
        if (!IsValid(ExistingItem))
            continue;

        if (ExistingItem->SectionIndex != InSectionIndex)
            continue;

        int32 ExWidth = ExistingItem->bRotated ? /*Height*/ 1 : /*Width*/ 1;
        int32 ExHeight = ExistingItem->bRotated ? /*Width*/ 1 : /*Height*/ 1;

        const bool bOverlap =
            InPosX < ExistingItem->PosX + ExWidth &&
            InPosX + ItemWidth > ExistingItem->PosX &&
            InPosY < ExistingItem->PosY + ExHeight &&
            InPosY + ItemHeight > ExistingItem->PosY;

        if (bOverlap)
        {
            return false;
        }
    }

    return true;
}

void UGridInventoryComponent::Server_AddItemToPos_Implementation(UItemInstance* NewItem, 
    const FGuid& ContainerGuid, 
    int32 InSectionIndex, 
    int32 InPosX, int32 InPosY)
{
    if (!IsValid(NewItem))
    {
        return;
    }

    RebuildItemOwnerMap();

    if (!CanAddItemToPos(
        NewItem,
        ContainerGuid,
        InSectionIndex,
        InPosX,
        InPosY))
    {
        return;
    }

    if (!Items.Contains(NewItem))
    {
        Items.Add(NewItem);
    }

    NewItem->OwnerItemGuid = ContainerGuid;
    NewItem->SectionIndex = InSectionIndex;
    NewItem->PosX = InPosX;
    NewItem->PosY = InPosY;

    TArray<UItemInstance*>& ItemsInContainer =
        ItemsByOwnerGuid.FindOrAdd(ContainerGuid);

    ItemsInContainer.Add(NewItem);

    RebuildItemOwnerMap();
}


bool UGridInventoryComponent::RemoveItem(UItemInstance* Item)
{
    if (!IsValid(Item))
    {
        return false;
    }

    if (GetOwnerRole() < ROLE_Authority)
    {
        Server_RemoveItem(Item);
    }
    else
    {
        Server_RemoveItem(Item);
    }

    return true;
}

void UGridInventoryComponent::Server_RemoveItem_Implementation(UItemInstance* Item)
{
    if (!IsValid(Item))
    {
        return;
    }
    RebuildItemOwnerMap();

    if (TArray<UItemInstance*>* ChildItems = ItemsByOwnerGuid.Find(Item->InstanceID))
    {
        TArray<UItemInstance*> ChildrenToRemove = *ChildItems;

        for (UItemInstance* Child : ChildrenToRemove)
        {
            if (!IsValid(Child))
            {
                continue;
            }

            Items.Remove(Child);
        }
    }

    Items.Remove(Item);
    
    Item->OwnerItemGuid.Invalidate();
    Item->SectionIndex = INDEX_NONE;
    Item->PosX = 0;
    Item->PosY = 0;

    RebuildItemOwnerMap();
}
