// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldActor/WorldActorSpawner.h"

// Sets default values for this component's properties
UWorldActorSpawner::UWorldActorSpawner()
{
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UWorldActorSpawner::BeginPlay()
{
	Super::BeginPlay();
	CollectSpawnPoints();
	
}

void UWorldActorSpawner::CollectSpawnPoints()
{
	SpawnDataList.Empty();

	AActor* Owner = GetOwner();
	if (!Owner) return;


	TArray<UActorComponent*> AllComponents;
	Owner->GetComponents(USceneComponent::StaticClass(), AllComponents);

	TArray<USceneComponent*> SubmitSpawnPoints;
	TArray<USceneComponent*> ItemSpawnPoints;
	for (UActorComponent* Comp : AllComponents)
	{
		USceneComponent* SceneComp = Cast<USceneComponent>(Comp);
		if (SceneComp){
			FActorSpawnData Data;
			if (SceneComp->ComponentHasTag(FName("SubmitSpawnPoint")))
			{
				Data.SpawnCategory = ESpawnCategory::Submit;
				Data.SpawnPoint = SceneComp;
				SpawnDataList.Add(Data);
			}
			else if (SceneComp->ComponentHasTag(FName("ItemSpawnPoint")))
			{				
				Data.SpawnCategory = ESpawnCategory::Item;
				Data.SpawnPoint = SceneComp;
				SpawnDataList.Add(Data);
			}
			
		}
	}
}

