// ItemBass.cpp

#include "Item/ItemBass.h"
#include "Components/SceneComponent.h"
#include "GameFramework/Character.h"
#include "GridInventory/ItemInstance/ItemInstance.h"

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
}

void AItemBass::Server_Use_Implementation(ACharacter* User)
{
    if (!HasAuthority()) return;
    if (!User) return;
    if (IsBroken()) return;

    ExecuteUse(User);

    ReduceDurability(1);

    if (IsBroken())
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

bool AItemBass::ReduceDurability(int32 Amount)
{
    if (!HasAuthority()) return false;
    if (Amount <= 0) return false;
    if (IsBroken()) return false;

    ItemInstanceComponent->GetItemInstance()->AddDurability(-Amount);
    return true;
}

void AItemBass::Equip(AActor* Equip_Owner)
{
}

void AItemBass::UpdateItemDataDurability(int32 NewDurability)
{
}

void AItemBass::ExecuteUse(ACharacter* User)
{
}