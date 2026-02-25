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

	// 스폰 큐 기반 몬스터 순차 스폰 시작
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

	// TODO: 임시 FloorLevel - 추후 GameMode 또는 던전 시스템에서 가져올 것
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnManager")
	int32 FloorLevel = 1;

private:
	FTimerHandle CheckTimerHandle;
	FTimerHandle SpawnTimerHandle;

	TArray<FMonsterSpawnData> SpawnQueue;
	int32 CurrentSpawnIndex = 0;

	// 던전 생성 완료 여부 폴링
	void CheckDungeonComplete();
	// 월드의 모든 MonsterSpawner에서 스폰 데이터 수집
	void CollectAllSpawnEntries();
	// 큐에서 다음 몬스터 스폰
	void SpawnNextInQueue();
};
