//Item_Spawn.cpp

#include "Item_Spawn/Item_Spawn.h"

#include "Components/BoxComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

AItem_Spawn::AItem_Spawn()
{
    PrimaryActorTick.bCanEverTick = false;

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Component"));
	SetRootComponent(SceneComp);

	SpawnBoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Spawn Box Component"));
	SpawnBoxComp->SetupAttachment(RootComponent);
}

FVector AItem_Spawn::GetRandomPointInVolume() const
{
	FVector BoxExtent = SpawnBoxComp->GetScaledBoxExtent();
	FVector BoxOrigin = SpawnBoxComp->GetComponentLocation();

	return BoxOrigin + FVector(
		FMath::FRandRange(-BoxExtent.X, BoxExtent.X),
		FMath::FRandRange(-BoxExtent.Y, BoxExtent.Y),
		FMath::FRandRange(-BoxExtent.Z, BoxExtent.Z)
	);
}

void AItem_Spawn::SpawnItem(TSubclassOf<AActor> ItemClass)
{
	if (!ItemClass) return;

	GetWorld()->SpawnActor<AActor>(
		ItemClass,
		GetRandomPointInVolume(),
		FRotator::ZeroRotator
	);
}
