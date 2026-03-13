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

	TArray<FActorSpawnData> SpawnQueue;

	int32 CurrentSpawnIndex = 0;
	int32 CurrentSpawnCost = 0;

	USceneComponent* LastSpawnPoint = nullptr;

	void CheckDungeonComplete();
	void CollectAllSpawnEntries();
	void SpawnNextInQueue();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnManager")
	int32 MaxSpawnCost = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnManager")
	UDataTable* ItemDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnManager")
	float SpawnRadius = 120.f;
};
