#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MonsterSpawner.h"
#include "Maps/NecDungeonsGenerator.h"
#include "MonsterSpawnManager.generated.h"


class AMonsterBase;

UCLASS()
class NECROMANCER_API AMonsterSpawnManager : public AActor
{
	GENERATED_BODY()

public:
	AMonsterSpawnManager();
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "SpawnManager")
	void StartSpawning();

public:
	// 에디터에서 레벨에 있는 DungeonGenerator를 연결
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "SpawnManager")
	ANecDungeonsGenerator* DungeonGenerator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnManager")
	float CheckInterval = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnManager")
	float SpawnDelay = 0.3f;

private:
	FTimerHandle CheckTimerHandle;
	FTimerHandle SpawnTimerHandle;

	TArray<FMonsterSpawnData> SpawnQueue;
	int32 CurrentSpawnIndex = 0;

	void CheckDungeonComplete();
	void CollectAllSpawnEntries();
	void SpawnNextInQueue();
};
