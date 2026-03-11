// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldActor/WorldActorSpawnManager.h"
#include "WorldActor/WorldActorSpawner.h"
#include "Maps/NecDungeonsGenerator.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "EngineUtils.h"

#include "GridInventory/ItemData/ItemDataSubsystem.h"
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

	CurrentSpawnCost = 0;

	if (SpawnQueue.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnManager: No Submit spawn entries found"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("SpawnManager: %d monsters queued"), SpawnQueue.Num());
	CurrentSpawnIndex = 0;

	if (SpawnDelay <= 0.0f)
	{
		for (int32 i = 0; i < SpawnQueue.Num(); i++)
		{
			const FActorSpawnData& Entry = SpawnQueue[i];

			if (Entry.SpawnPoint)
			{
				FVector Location = Entry.SpawnPoint->GetComponentLocation();
				FRotator Rotation = Entry.SpawnPoint->GetComponentRotation();

				FActorSpawnParameters Params;
				Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

				UWorld* World = GetWorld();
				if (!World) {
					UE_LOG(LogTemp, Warning, TEXT("Spawn failed: World is null"));
					CurrentSpawnIndex++;
					return;
				}
				UGameInstance* GI = World->GetGameInstance();
				if (!GI) {
					UE_LOG(LogTemp, Warning, TEXT("Spawn failed: GameInstance is null"));
					CurrentSpawnIndex++;
					return;
				}
				UDataTableSubsystem* Subsystem = GI->GetSubsystem<UDataTableSubsystem>();
				if (!Subsystem) {
					UE_LOG(LogTemp, Warning, TEXT("Spawn failed: ItemDataSubsystem is null"));
					CurrentSpawnIndex++;
					return;
				}

				AActor* SubmitActor = nullptr;

				switch (Entry.SpawnCategory)
				{
				case ESpawnCategory::Submit:
				{
					// Submit 전용 로직
					const  FworldActorInfo* WorldActorInfo = Subsystem->GetworldActorInfo("Bucket");
					if (!WorldActorInfo)
					{
						UE_LOG(LogTemp, Warning, TEXT("Spawn failed: WorldActorInfo is null"));
						break;
					}

					if (!WorldActorInfo->WorldActorClass)
					{
						UE_LOG(LogTemp, Warning, TEXT("Spawn failed: WorldActorClass is null"));
						break;
					}

					SubmitActor = World->SpawnActor<AActor>(
						WorldActorInfo->WorldActorClass,
						Location,
						Rotation,
						Params
					);
					break;
				}
				case ESpawnCategory::Item:
				{
					// Item 전용 로직
					const FItemData* ItemData = Subsystem->GetRandomItemData();
					if (!ItemData)
					{
						UE_LOG(LogTemp, Warning, TEXT("Spawn failed: ItemData is null"));
						break;
					}

					if (CurrentSpawnCost + ItemData->SpawnCost > MaxSpawnCost)
					{
						UE_LOG(LogTemp, Warning, TEXT("Spawn skipped: Cost limit reached (%d/%d)"),
							CurrentSpawnCost, MaxSpawnCost);
						break;
					}

					if (!ItemData->DropItemActorClass)
					{
						UE_LOG(LogTemp, Warning, TEXT("Spawn failed: DropItemActorClass is null"));
						break;
					}

					SubmitActor = World->SpawnActor<AActor>(
						ItemData->DropItemActorClass,
						Location,
						Rotation,
						Params
					);
					break;
				}
				default:
				{
					UE_LOG(LogTemp, Warning, TEXT("Unknown SpawnCategory"));
					break;
				}
				}
			}
		}
		for (int32 i = 0; i < SpawnQueue.Num(); i++)
		{
			CurrentSpawnIndex = i;
		}
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &AWorldActorSpawnManager::SpawnNextInQueue, SpawnDelay, true);
	}
}

void AWorldActorSpawnManager::CollectAllSpawnEntries()
{
	SpawnQueue.Empty();

	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		UWorldActorSpawner* Spawner = It->FindComponentByClass<UWorldActorSpawner>();
		if (Spawner && !Spawner->bHasSpawned)
		{
			SpawnQueue.Append(Spawner->SpawnDataList);
			Spawner->bHasSpawned = true;
		}
	}
}

void AWorldActorSpawnManager::SpawnNextInQueue()
{
	if (CurrentSpawnIndex >= SpawnQueue.Num())
	{
		GetWorld()->GetTimerManager().ClearTimer(SpawnTimerHandle);
		UE_LOG(LogTemp, Log, TEXT("SpawnManager: All %d submitActor spawned"), SpawnQueue.Num());
		return;
	}

	FActorSpawnData& SpawnData = SpawnQueue[CurrentSpawnIndex];
	const FActorSpawnData& Entry = SpawnQueue[CurrentSpawnIndex];

	if (LastSpawnPoint != Entry.SpawnPoint)
	{
		CurrentSpawnCost = 0;
		LastSpawnPoint = Entry.SpawnPoint;
	}

	if (Entry.SpawnPoint)
	{
		FVector Location = Entry.SpawnPoint->GetComponentLocation();
		FRotator Rotation = Entry.SpawnPoint->GetComponentRotation();

		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		UWorld* World = GetWorld();
		if (!World)	{
			UE_LOG(LogTemp, Warning, TEXT("Spawn failed: World is null"));
			CurrentSpawnIndex++;
			return;
		}
		UGameInstance* GI = World->GetGameInstance();
		if (!GI){
			UE_LOG(LogTemp, Warning, TEXT("Spawn failed: GameInstance is null"));
			CurrentSpawnIndex++;
			return;
		}
		UDataTableSubsystem* Subsystem = GI->GetSubsystem<UDataTableSubsystem>();
		if (!Subsystem){
			UE_LOG(LogTemp, Warning, TEXT("Spawn failed: ItemDataSubsystem is null"));
			CurrentSpawnIndex++;
			return;
		}

		AActor* SubmitActor = nullptr;

		switch (Entry.SpawnCategory)
		{
		case ESpawnCategory::Submit:
		{
			// Submit 전용 로직
			const  FworldActorInfo* WorldActorInfo = Subsystem->GetworldActorInfo("Bucket");
			if (!WorldActorInfo)
			{
				UE_LOG(LogTemp, Warning, TEXT("Spawn failed: WorldActorInfo is null"));
				break;
			}

			if (!WorldActorInfo->WorldActorClass)
			{
				UE_LOG(LogTemp, Warning, TEXT("Spawn failed: WorldActorClass is null"));
				break;
			}

			SubmitActor = World->SpawnActor<AActor>(
				WorldActorInfo->WorldActorClass,
				Location,
				Rotation,
				Params
			);
			break;
		}
		case ESpawnCategory::Item:
		{
			const FItemData* ItemData = Subsystem->GetRandomItemData();
			if (!ItemData)
			{
				UE_LOG(LogTemp, Warning, TEXT("Spawn failed: ItemData is null"));
				break;
			}

			if (CurrentSpawnCost + ItemData->SpawnCost > MaxSpawnCost)
			{
				UE_LOG(LogTemp, Warning, TEXT("Spawn skipped: Cost limit reached (%d/%d)"),
					CurrentSpawnCost, MaxSpawnCost);
				CurrentSpawnIndex++;
				return;
			}

			if (!ItemData->DropItemActorClass)
			{
				UE_LOG(LogTemp, Warning, TEXT("Spawn failed: DropItemActorClass is null"));
				break;
			}

			SubmitActor = World->SpawnActor<AActor>(
				ItemData->DropItemActorClass,
				Location,
				Rotation,
				Params
			);
			if (SubmitActor)
			{
				CurrentSpawnCost += ItemData->SpawnCost;
			}
			break;
		}
		default:
		{
			UE_LOG(LogTemp, Warning, TEXT("Unknown SpawnCategory"));
			break;
			}
		}		

		if (SubmitActor)
		{
			UE_LOG(LogTemp, Log, TEXT("Spawned [%d/%d] Cost:%d/%d"),CurrentSpawnIndex + 1,SpawnQueue.Num(),CurrentSpawnCost,MaxSpawnCost);
		}
	}
	CurrentSpawnIndex++;
}


