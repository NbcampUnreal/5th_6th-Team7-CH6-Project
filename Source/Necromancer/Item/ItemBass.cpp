//ItemBass.cpp

#include "Item/ItemBass.h"
#include "Components/SceneComponent.h"
#include "GameFramework/Character.h"

AItemBass::AItemBass()
{
    PrimaryActorTick.bCanEverTick = false;

    bReplicates = true;
    SetReplicateMovement(true);

    USceneComponent* SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
    SetRootComponent(SceneComp);

    ItemInstanceComponent = CreateDefaultSubobject<UItemInstanceComponent>(TEXT("ItemInstanceComponent"));
}

void AItemBass::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AItemBass, ItemData);
}

void AItemBass::Server_Use_Implementation(ACharacter* User)
{
    if (!HasAuthority()) return;
    if (!User) return;
    if (IsEmpty()) return;

    ExecuteUse(User);

    RemoveCount(1);

    if (IsEmpty())
    {
        Destroy();
    }
}

void AItemBass::Server_Drop_Implementation(const FVector& DropLocation)
{
    if (!HasAuthority()) return;

    SetActorLocation(DropLocation);
    SetActorHiddenInGame(false);
    SetActorEnableCollision(true);
}

bool AItemBass::AddCount(int32 Amount)
{
    if (!HasAuthority()) return false;
    if (Amount <= 0) return false;

    ItemData.Count += Amount;
    return true;
}

bool AItemBass::RemoveCount(int32 Amount)
{
    if (!HasAuthority()) return false;
    if (Amount <= 0 || ItemData.Count <= 0) return false;

    ItemData.Count -= Amount;

    if (ItemData.Count < 0)
    {
        ItemData.Count = 0;
    }

    return true;
}

void AItemBass::ExecuteUse(ACharacter* User)
{
}
