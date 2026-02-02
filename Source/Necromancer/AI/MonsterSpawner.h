#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MonsterSpawner.generated.h"

class AMonsterBase;

USTRUCT(BlueprintType)
struct FMonsterSpawnData
{
	GENERATED_BODY()

	UPROPERTY()
	TSubclassOf<AMonsterBase> MonsterClass;

	UPROPERTY()
	TObjectPtr<USceneComponent> SpawnPoint = nullptr;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NECROMANCER_API UMonsterSpawner : public UActorComponent
{
	GENERATED_BODY()

public:
	UMonsterSpawner();
	virtual void BeginPlay() override;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
	TArray<TSubclassOf<AMonsterBase>> MonsterClasses;

	
	UPROPERTY(BlueprintReadOnly, Category = "Spawner")
	TArray<FMonsterSpawnData> SpawnDataList;

	UPROPERTY(BlueprintReadOnly, Category = "Spawner")
	bool bHasSpawned = false;

private:
	
	void CollectSpawnPoints();
};
