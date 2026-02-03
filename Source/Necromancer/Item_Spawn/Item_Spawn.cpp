//Item_Spawn.cpp

#include "Item_Spawn/Item_Spawn.h"

#include "Components/BoxComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GridInventory/ItemData/ItemDataSubsystem.h"

AItem_Spawn::AItem_Spawn()
{
    PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

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

void AItem_Spawn::SpawnItem(FName ItemID)
{
	// 클라에서 불리면 서버로 요청
	if (!HasAuthority())
	{
		Server_SpawnItem(ItemID);
		return;
	}

	// 서버면 바로 스폰
	Internal_SpawnItem(ItemID);
}

void AItem_Spawn::Internal_SpawnItem(FName ItemID)
{
	UItemDataSubsystem* Subsystem = GetOwner()->GetGameInstance()->GetSubsystem<UItemDataSubsystem>();
	const FItemData* Data = Subsystem->GetItemData(ItemID);

	GetWorld()->SpawnActor<AActor>(
		Data->DropItemActorClass,
		GetRandomPointInVolume(),
		FRotator::ZeroRotator
	);
}


void AItem_Spawn::Server_SpawnItem_Implementation(FName ItemID)
{
	Internal_SpawnItem(ItemID);
}
