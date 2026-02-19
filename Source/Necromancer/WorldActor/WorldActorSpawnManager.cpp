// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldActor/WorldActorSpawnManager.h"
#include "WorldActor/WorldActorSpawner.h"
#include "Maps/NecDungeonsGenerator.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "EngineUtils.h"

// Sets default values
AWorldActorSpawnManager::AWorldActorSpawnManager()
{
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AWorldActorSpawnManager::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
	{
		GetWorld()->GetTimerManager().SetTimer(CheckTimerHandle, this, &AWorldActorSpawnManager::CheckDungeonComplete, CheckInterval, true);
	}
}

void AWorldActorSpawnManager::CheckDungeonComplete()
{
	if (DungeonGenerator && DungeonGenerator->GetbIsDungeonComplete())
	{
		GetWorld()->GetTimerManager().ClearTimer(CheckTimerHandle);
		StartSpawning();
	}
}

void AWorldActorSpawnManager::StartSpawning()
{
	CollectAllSpawnEntries();

	if (SubmitSpawnQueue.Num() == 0&& ItemSpawnQueue.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnManager: No Submit spawn entries found"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("SpawnManager: %d monsters queued"), SubmitSpawnQueue.Num());
	CurrentSubmitSpawnIndex = 0;
	CurrentItemSpawnIndex = 0;

	if (SpawnDelay <= 0.0f)
	{
		for (int32 i = 0; i < SubmitSpawnQueue.Num(); i++)
		{
			CurrentSubmitSpawnIndex = i;
			SubmitSpawnNextInQueue();
		}
		for (int32 i = 0; i < ItemSpawnQueue.Num(); i++)
		{
			CurrentItemSpawnIndex = i;
			ItemSpawnNextInQueue();
		}
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &AWorldActorSpawnManager::SubmitSpawnNextInQueue, SpawnDelay, true);
		GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &AWorldActorSpawnManager::ItemSpawnNextInQueue, SpawnDelay, true);
	}
}

void AWorldActorSpawnManager::CollectAllSpawnEntries()
{
	SubmitSpawnQueue.Empty();

	// 월드의 모든 액터에서 UMonsterSpawner 컴포넌트를 찾기
	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		UWorldActorSpawner* Spawner = It->FindComponentByClass<UWorldActorSpawner>();
		if (Spawner && !Spawner->bHasSpawned)
		{
			SubmitSpawnQueue.Append(Spawner->SubmitSpawnDataList);
			ItemSpawnQueue.Append(Spawner->ItemSpawnDataList);
			Spawner->bHasSpawned = true;
		}
	}
}

void AWorldActorSpawnManager::SubmitSpawnNextInQueue()
{
	if (CurrentSubmitSpawnIndex >= SubmitSpawnQueue.Num())
	{
		GetWorld()->GetTimerManager().ClearTimer(SpawnTimerHandle);
		UE_LOG(LogTemp, Log, TEXT("SpawnManager: All %d submitActor spawned"), SubmitSpawnQueue.Num());
		return;
	}

	const FActorSpawnData& Entry = SubmitSpawnQueue[CurrentSubmitSpawnIndex];

	if (Entry.WorldActorClass && Entry.SpawnPoint)
	{
		FVector Location = Entry.SpawnPoint->GetComponentLocation();
		FRotator Rotation = Entry.SpawnPoint->GetComponentRotation();

		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		AActor* SubmitActor = GetWorld()->SpawnActor<AActor>(Entry.WorldActorClass, Location, Rotation, Params);

		if (SubmitActor)
		{
			UE_LOG(LogTemp, Log, TEXT("Spawned [%d/%d]: %s"), CurrentSubmitSpawnIndex + 1, SubmitSpawnQueue.Num(), *Entry.WorldActorClass->GetName());
		}
	}

	CurrentSubmitSpawnIndex++;
}

void AWorldActorSpawnManager::ItemSpawnNextInQueue()
{
	if (CurrentItemSpawnIndex >= ItemSpawnQueue.Num())
	{
		GetWorld()->GetTimerManager().ClearTimer(SpawnTimerHandle);
		UE_LOG(LogTemp, Log, TEXT("SpawnManager: All %d submitActor spawned"), ItemSpawnQueue.Num());
		return;
	}

	const FActorSpawnData& Entry = ItemSpawnQueue[CurrentItemSpawnIndex];

	if (Entry.WorldActorClass && Entry.SpawnPoint)
	{
		FVector Location = Entry.SpawnPoint->GetComponentLocation();
		FRotator Rotation = Entry.SpawnPoint->GetComponentRotation();

		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		AActor* SubmitActor = GetWorld()->SpawnActor<AActor>(Entry.WorldActorClass, Location, Rotation, Params);

		if (SubmitActor)
		{
			UE_LOG(LogTemp, Log, TEXT("Spawned [%d/%d]: %s"), CurrentItemSpawnIndex + 1, ItemSpawnQueue.Num(), *Entry.WorldActorClass->GetName());
		}
	}

	CurrentItemSpawnIndex++;
}


