// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Maps/NecDungeonsGenerator.h"
#include "WorldActor/WorldActorSpawner.h"
#include "WorldActorSpawnManager.generated.h"

UCLASS()
class NECROMANCER_API AWorldActorSpawnManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWorldActorSpawnManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "SpawnManager")
	void StartSpawning();
public:	
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "SpawnManager")
	ANecDungeonsGenerator* DungeonGenerator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnManager")
	float CheckInterval = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnManager")
	float SpawnDelay = 0.3f;

private:
	FTimerHandle CheckTimerHandle;
	FTimerHandle SpawnTimerHandle;

	TArray<FActorSpawnData> SubmitSpawnQueue;
	TArray<FActorSpawnData> ItemSpawnQueue;

	int32 CurrentSubmitSpawnIndex = 0;
	int32 CurrentItemSpawnIndex = 0;

	void CheckDungeonComplete();
	void CollectAllSpawnEntries();
	void SubmitSpawnNextInQueue();
	void ItemSpawnNextInQueue();
};
