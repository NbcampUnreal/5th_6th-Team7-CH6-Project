#include "Item/Item_Consumption_Base/Throwing_Item_Base.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

void UThrowing_Item_Base::ExecuteUseAction(ACharacter* User)
{
    if (!User || !ProjectileClass)
        return;

    UWorld* World = User->GetWorld();
    if (!World)
        return;

    FVector SpawnLocation = User->GetActorLocation() + User->GetActorForwardVector() * 100.f;
    FRotator SpawnRotation = User->GetControlRotation();

    FActorSpawnParameters Params;
    Params.Owner = User;
    Params.Instigator = User;

    AActor* SpawnedProjectile = World->SpawnActor<AActor>(
        ProjectileClass,
        SpawnLocation,
        SpawnRotation,
        Params
    );

    if (SpawnedProjectile)
    {
        UPrimitiveComponent* Primitive = Cast<UPrimitiveComponent>(
            SpawnedProjectile->GetComponentByClass(UPrimitiveComponent::StaticClass())
        );

        if (Primitive && Primitive->IsSimulatingPhysics())
        {
            Primitive->AddImpulse(User->GetActorForwardVector() * ThrowForce);
        }
    }
}