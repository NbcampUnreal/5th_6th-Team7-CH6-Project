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
	SubmitSpawnDataList.Empty();

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
			if (SceneComp->ComponentHasTag(FName("SubmitSpawnPoint")))
			{
				SubmitSpawnPoints.Add(SceneComp);
			}
			if (SceneComp->ComponentHasTag(FName("ItemSpawnPoint")))
			{
				ItemSpawnPoints.Add(SceneComp);
			}
		}
	}


	int32 Count = FMath::Min(SubmmitActorClasses.Num(), SubmitSpawnPoints.Num());
	for (int32 i = 0; i < Count; i++)
	{
		if (SubmmitActorClasses[i])
		{
			FActorSpawnData Data;
			Data.WorldActorClass = SubmmitActorClasses[i];
			Data.SpawnPoint = SubmitSpawnPoints[i];
			SubmitSpawnDataList.Add(Data);
		}
	}
	Count = FMath::Min(ItemActorClasses.Num(), ItemSpawnPoints.Num());
	for (int32 i = 0; i < Count; i++)
	{
		if (ItemActorClasses[i])
		{
			FActorSpawnData Data;
			Data.WorldActorClass = ItemActorClasses[i];
			Data.SpawnPoint = ItemSpawnPoints[i];
			ItemSpawnDataList.Add(Data);
		}
	}
	UE_LOG(LogTemp, Log, TEXT("MonsterSpawner [%s]: Found %d SpawnPoints, %d MonsterClasses, %d matched"), *Owner->GetName(), SubmitSpawnPoints.Num(), SubmmitActorClasses.Num(), SubmitSpawnDataList.Num());
}

