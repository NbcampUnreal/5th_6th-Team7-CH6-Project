// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WorldActorSpawner.generated.h"

UENUM()
enum class ESpawnCategory : uint8
{
	Submit,
	Item,
	Etc
};

USTRUCT(BlueprintType)
struct FActorSpawnData
{
	GENERATED_BODY()

	UPROPERTY()
	ESpawnCategory SpawnCategory;

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
public:	
	UPROPERTY(BlueprintReadOnly, Category = "Spawner")
	TArray<FActorSpawnData> SpawnDataList;

	UPROPERTY(BlueprintReadOnly, Category = "Spawner")
	bool bHasSpawned = false;
private:

	void CollectSpawnPoints();		
};
