// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WorldActorSpawner.generated.h"

USTRUCT(BlueprintType)
struct FActorSpawnData
{
	GENERATED_BODY()

	UPROPERTY()
	TSubclassOf<AActor> WorldActorClass;

	UPROPERTY()
	TObjectPtr<USceneComponent> SpawnPoint = nullptr;
};

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NECROMANCER_API UWorldActorSpawner : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWorldActorSpawner();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
	TArray<TSubclassOf<AActor>> SubmmitActorClasses;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
	TArray<TSubclassOf<AActor>> ItemActorClasses;
public:	
	UPROPERTY(BlueprintReadOnly, Category = "Spawner")
	TArray<FActorSpawnData> SubmitSpawnDataList;

	UPROPERTY(BlueprintReadOnly, Category = "Spawner")
	TArray<FActorSpawnData> ItemSpawnDataList;

	UPROPERTY(BlueprintReadOnly, Category = "Spawner")
	bool bHasSpawned = false;
private:

	void CollectSpawnPoints();		
};
