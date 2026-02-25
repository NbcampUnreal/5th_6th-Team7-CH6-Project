#include "AI/MonsterSpawnManager.h"
#include "AI/MonsterSpawner.h"
#include "AI/MonsterBase.h"
#include "AI/MonsterStatComponent.h"
#include "Maps/NecDungeonsGenerator.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "EngineUtils.h"

AMonsterSpawnManager::AMonsterSpawnManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AMonsterSpawnManager::BeginPlay()
{
	Super::BeginPlay();

	
	if (!HasAuthority())
	{
		return;
	}

	GetWorld()->GetTimerManager().SetTimer(CheckTimerHandle,this,&AMonsterSpawnManager::CheckDungeonComplete,CheckInterval,true);
}

void AMonsterSpawnManager::CheckDungeonComplete()
{
	if (DungeonGenerator && DungeonGenerator->GetbIsDungeonComplete())
	{
		GetWorld()->GetTimerManager().ClearTimer(CheckTimerHandle);
		StartSpawning();
	}
}

void AMonsterSpawnManager::StartSpawning()
{
	CollectAllSpawnEntries();

	if (SpawnQueue.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnManager: No spawn entries found"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("SpawnManager: %d monsters queued"), SpawnQueue.Num());
	CurrentSpawnIndex = 0;

	if (SpawnDelay <= 0.0f)
	{
		for (int32 i = 0; i < SpawnQueue.Num(); i++)
		{
			CurrentSpawnIndex = i;
			SpawnNextInQueue();
		}
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle,this,&AMonsterSpawnManager::SpawnNextInQueue,SpawnDelay,true);
	}
}

void AMonsterSpawnManager::CollectAllSpawnEntries()
{
	SpawnQueue.Empty();

	// 월드의 모든 액터에서 UMonsterSpawner 컴포넌트를 찾기
	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		UMonsterSpawner* Spawner = It->FindComponentByClass<UMonsterSpawner>();
		if (Spawner && !Spawner->bHasSpawned)
		{
			SpawnQueue.Append(Spawner->SpawnDataList);
			Spawner->bHasSpawned = true;
		}
	}
}

void AMonsterSpawnManager::SpawnNextInQueue()
{
	if (CurrentSpawnIndex >= SpawnQueue.Num())
	{
		GetWorld()->GetTimerManager().ClearTimer(SpawnTimerHandle);
		UE_LOG(LogTemp, Log, TEXT("SpawnManager: All %d monsters spawned"), SpawnQueue.Num());
		return;
	}

	const FMonsterSpawnData& Entry = SpawnQueue[CurrentSpawnIndex];

	if (Entry.MonsterClass && Entry.SpawnPoint)
	{
		FVector Location = Entry.SpawnPoint->GetComponentLocation();
		FRotator Rotation = Entry.SpawnPoint->GetComponentRotation();

		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		AMonsterBase* Monster = GetWorld()->SpawnActor<AMonsterBase>(Entry.MonsterClass, Location, Rotation, Params);

		if (Monster)
		{
			// 층별 스탯 스케일링 적용
			if (UMonsterStatComponent* StatComp = Monster->FindComponentByClass<UMonsterStatComponent>())
			{
				StatComp->ApplyFloorScaling(FloorLevel);
			}

			UE_LOG(LogTemp, Log, TEXT("Spawned [%d/%d]: %s (Floor:%d)"),CurrentSpawnIndex + 1,SpawnQueue.Num(),*Entry.MonsterClass->GetName(), FloorLevel);
		}
	}

	CurrentSpawnIndex++;
}
