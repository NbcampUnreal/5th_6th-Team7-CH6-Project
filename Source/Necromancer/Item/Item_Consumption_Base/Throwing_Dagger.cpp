//Throwing_Dagger.cpp

#include "Item/Item_Consumption_Base/Throwing_Dagger.h"
#include "GameFramework/Character.h"
#include "Engine/World.h"

void UThrowing_Dagger::ExecuteUseAction(ACharacter* User)
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

    World->SpawnActor<AActor>(
        ProjectileClass,
        SpawnLocation,
        SpawnRotation,
        Params
    );
}