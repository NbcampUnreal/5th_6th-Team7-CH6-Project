//Item_Consumption_Bass.cpp

#include "Item/Item_Consumption_Bass.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

AItem_Consumption_Bass::AItem_Consumption_Bass()
{
    ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
    ItemMesh->SetupAttachment(RootComponent);

    ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    ItemMesh->SetSimulatePhysics(true);
}

void AItem_Consumption_Bass::ExecuteUse(ACharacter* User)
{
    if (!HasAuthority()) return;

    Multicast_PlayUseEffects();
}

void AItem_Consumption_Bass::Multicast_PlayUseEffects_Implementation()
{
    if (UseSound)
    {
        UGameplayStatics::PlaySoundAtLocation(
            this,
            UseSound,
            GetActorLocation()
        );
    }

    if (ItemMesh)
    {
        ItemMesh->SetVisibility(false);
    }
}
