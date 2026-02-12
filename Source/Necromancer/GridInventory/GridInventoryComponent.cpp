// Fill out your copyright notice in the Description page of Project Settings.


#include "GridInventory/GridInventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "GridInventory/ItemInstance/ItemInstance.h"
#include "GridInventory/ItemData/ItemDataSubsystem.h"
#include "Engine/ActorChannel.h"

// Sets default values for this component's properties
UGridInventoryComponent::UGridInventoryComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);
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

void UGridInventoryComponent::OnRep_Items()
{
    if (bInventoryActive)
    {
        RebuildItemOwnerMap();
        OnInventoryUpdated.Broadcast();
    }
}

void UGridInventoryComponent::HandleItemChanged(UItemInstance* Item)
{
    RebuildItemOwnerMap();
    OnInventoryUpdated.Broadcast();
}

void UGridInventoryComponent::SetInventory(const TArray<UItemInstance*>& InItems) {
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

void UGridInventoryComponent::GetInventory(TArray<UItemInstance*>& OutItems) const
{
    OutItems = Items;
}

bool UGridInventoryComponent::FindInventoryContainer(
    FGuid ContainerId, 
    TArray<UItemInstance*>& OutItems)
{
    OutItems.Reset();

    if (TArray<UItemInstance*>* ItemsInContainer =
        ItemsByOwnerGuid.Find(ContainerId))
    {
        OutItems = *ItemsInContainer;
        return true;
    }
    return false;
}

void UGridInventoryComponent::AddRootItem(UItemInstance* NewItem)
{
    if (!IsValid(NewItem))
    {
        return;
    }
    if (GetOwnerRole() < ROLE_Authority)
    {
        Server_AddRootItem(
            NewItem
        );
    }
    else
    {
        Implement_AddRootItem(
            NewItem
        );
    }

    RebuildItemOwnerMap();
    OnInventoryUpdated.Broadcast();
    UE_LOG(LogTemp, Warning, TEXT("AddNecInventory: ItemInstance is In"));
}

bool UGridInventoryComponent::AddItemToPos(UItemInstance* NewItem,
    const FGuid& ContainerGuid, 
    int32 InRowIndex,
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
        InRowIndex,
        InSectionIndex,
        InPosX,
        InPosY))
    {
        UE_LOG(LogTemp, Warning, TEXT("AddNecInventory: ItemInstance is InCorret Position"));
        return false;
    }

    if (GetOwnerRole() < ROLE_Authority)
    {
        Server_AddItemToPos(
            NewItem,
            ContainerGuid,
            InRowIndex,
            InSectionIndex,
            InPosX,
            InPosY
        );
    }
    else
    {
        Implement_AddItemToPos(
            NewItem,
            ContainerGuid,
            InRowIndex,
            InSectionIndex,
            InPosX,
            InPosY
        );
    }

    RebuildItemOwnerMap();
    OnInventoryUpdated.Broadcast();
    UE_LOG(
        LogTemp,
        Warning,
        TEXT("AddNecInventory: Item [%s] -> Container [%s] | Section=%d Pos=(%d, %d)"),
        *NewItem->InstanceID.ToString(),
        *ContainerGuid.ToString(),
        InSectionIndex,
        InPosX,
        InPosY
    );
    return true;
}

bool UGridInventoryComponent::AddItemToContainer(
    UItemInstance* NewItem, 
    const FGuid& ContainerGuid)
{
    if (!IsValid(NewItem))
    {
        return false;
    }
    

    int32 InPosX = 0; int32 InPosY = 0;
    int32 InSectionIndex = 0;  int32 InRowIndex = 0;
    
    if (!CanAddToConatiner(NewItem,
        ContainerGuid,
        InRowIndex,
        InSectionIndex,
        InPosX,
        InPosY)) {
        return false;
    }

    if (GetOwnerRole() < ROLE_Authority)
    {
        Server_AddItemToPos(
            NewItem,
            ContainerGuid,
            InRowIndex,
            InSectionIndex,
            InPosX,
            InPosY
        );
    }
    else
    {
        Implement_AddItemToPos(
            NewItem,
            ContainerGuid,
            InRowIndex,
            InSectionIndex,
            InPosX,
            InPosY
        );
    }
    RebuildItemOwnerMap();
    OnInventoryUpdated.Broadcast();
    UE_LOG(
        LogTemp,
        Warning,
        TEXT("AddNecInventory: Item [%s] -> Container [%s] | Section=%d Pos=(%d, %d)"),
        *NewItem->InstanceID.ToString(),
        *ContainerGuid.ToString(),
        InSectionIndex,
        InPosX,
        InPosY
    );
    return true;
}

bool UGridInventoryComponent::CanAddItemToPos(UItemInstance* NewItem, 
    const FGuid& ContainerGuid,
    int32 InRowIndex,
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
        if (Item->InstanceID == ContainerGuid)
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
    UItemDataSubsystem* Subsystem = GetOwner()->GetGameInstance()->GetSubsystem<UItemDataSubsystem>();
    const FItemData* ItemData = Subsystem->GetItemData(NewItem->ItemID);
    const FItemData* ContainerData = Subsystem->GetItemData(ContainerItem->ItemID);
    if (!ItemData||!ContainerData) {
        return false;
    }

    if (ContainerData->Rows.Num() < 1)
    {
        return false;
    }

    /* 3. 섹션 인덱스 체크 */
    if (!ContainerData->Rows[InRowIndex].Sections.IsValidIndex(InSectionIndex))
    {
        return false;
    }

    const FInventorySection& Section =
        ContainerData->Rows[InRowIndex].Sections[InSectionIndex];

    /* 4. 아이템 크기 계산 (회전 고려) */
    int32 ItemWidth = NewItem->bRotated ? /*Height*/ ItemData->Height : /*Width*/ ItemData->Width;
    int32 ItemHeight = NewItem->bRotated ? /*Width*/ ItemData->Width : /*Height*/ ItemData->Height;

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

        if (ExistingItem->RowIndex != InRowIndex)
            continue;
        if (ExistingItem->SectionIndex != InSectionIndex)
            continue;
        if (ExistingItem->InstanceID == NewItem->InstanceID)
            continue;

        const FItemData* ExistingItemData = Subsystem->GetItemData(NewItem->ItemID);
        int32 ExWidth = ExistingItem->bRotated ? /*Height*/ ExistingItemData->Height : /*Width*/ ExistingItemData->Width;
        int32 ExHeight = ExistingItem->bRotated ? /*Width*/ ExistingItemData->Width : /*Height*/ ExistingItemData->Height;

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

void UGridInventoryComponent::Server_AddRootItem_Implementation(UItemInstance* NewItem)
{
    Implement_AddRootItem(NewItem);
}

void UGridInventoryComponent::Implement_AddRootItem(UItemInstance*& NewItem)
{    
    if (!Items.Contains(NewItem)) {
        Items.Add(NewItem);
    }
    NewItem->OwnerItemGuid = FGuid();

   //RebuildItemOwnerMap();
   //OnInventoryUpdated.Broadcast();
}

void UGridInventoryComponent::Server_AddItemToPos_Implementation(UItemInstance* NewItem, 
    const FGuid& ContainerGuid, 
    int32 InRowIndex,
    int32 InSectionIndex, 
    int32 InPosX, int32 InPosY)
{
    Implement_AddItemToPos(NewItem, ContainerGuid, InRowIndex, InSectionIndex, InPosX, InPosY);
}

void UGridInventoryComponent::Implement_AddItemToPos(
    UItemInstance*& NewItem, 
    const FGuid& ContainerGuid, 
    int32 InRowIndex,
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
        InRowIndex,
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
    NewItem->RowIndex = InRowIndex;
    NewItem->PosX = InPosX;
    NewItem->PosY = InPosY;

    //RebuildItemOwnerMap();
    //OnInventoryUpdated.Broadcast();
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
        Implement_RemoveItem(Item);
    }
    RebuildItemOwnerMap();
    OnInventoryUpdated.Broadcast();
    return true;
}

bool UGridInventoryComponent::CanAddToConatiner(UItemInstance* NewItem, const FGuid& ContainerGuid, int32& OutRowIndex, int32& OutSectionIndex, int32& OutPosX, int32& OutPosY)
{
    UItemInstance* ContainerItem = nullptr;
    for (UItemInstance* Item : Items)
    {
        if (Item->InstanceID == ContainerGuid)
        {
            ContainerItem = Item;
            break;
        }
    }
    if (!ContainerItem) {
        return false;
    }

    UItemDataSubsystem* Subsystem = GetOwner()->GetGameInstance()->GetSubsystem<UItemDataSubsystem>();
    const FItemData* Data = Subsystem->GetItemData(NewItem->ItemID);
    const FItemData* ContainerData = Subsystem->GetItemData(ContainerItem->ItemID);
    if (!Data || !ContainerData) {
        return false;
    }
    if (ContainerData->Rows.Num() < 1)
    {
        return false;
    }
    int32 InPosX = 0; int32 InPosY = 0;
    int32 RowIndex = ContainerData->Rows.Num();
    int32 InRowIndex = 0;
    bool bFoundPos = false;


    int32 SectionIndex = ContainerData->Rows[InRowIndex].Sections.Num();
    int32 InSectionIndex = 0;


    for (; InRowIndex < RowIndex; InRowIndex++) {
        for (; InSectionIndex < SectionIndex; InSectionIndex++) {
            for (InPosX = 0; InPosX < ContainerData->Rows[InRowIndex].Sections[InSectionIndex].Width; InPosX++) {
                for (InPosY = 0; InPosY < ContainerData->Rows[InRowIndex].Sections[InSectionIndex].Height; InPosY++) {
                    if (CanAddItemToPos(
                        NewItem,
                        ContainerGuid,
                        InRowIndex,
                        InSectionIndex,
                        InPosX,
                        InPosY))
                    {
                        OutRowIndex = InRowIndex;
                        OutSectionIndex = InSectionIndex;
                        OutPosX = InPosX;
                        OutPosY = InPosY;
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

void UGridInventoryComponent::Server_RemoveItem_Implementation(UItemInstance* Item)
{
    Implement_RemoveItem(Item);
}

void UGridInventoryComponent::Implement_RemoveItem(UItemInstance*& Item)
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
    Item->RowIndex = INDEX_NONE;
    Item->PosX = 0;
    Item->PosY = 0;

    //RebuildItemOwnerMap();
    //OnInventoryUpdated.Broadcast();
}
