#include "AI/MonsterSpawner.h"
#include "AI/MonsterBase.h"
#include "Necromancer.h"

UMonsterSpawner::UMonsterSpawner()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UMonsterSpawner::BeginPlay()
{
	Super::BeginPlay();
	CollectSpawnPoints();
}

void UMonsterSpawner::CollectSpawnPoints()
{
	SpawnDataList.Empty();

	AActor* Owner = GetOwner();
	if (!Owner) return;

	
	TArray<UActorComponent*> AllComponents;
	Owner->GetComponents(USceneComponent::StaticClass(), AllComponents);

	TArray<USceneComponent*> SpawnPoints;
	for (UActorComponent* Comp : AllComponents)
	{
		USceneComponent* SceneComp = Cast<USceneComponent>(Comp);
		if (SceneComp && SceneComp->ComponentHasTag(FName("SpawnPoint")))
		{
			SpawnPoints.Add(SceneComp);
		}
	}

	
	int32 Count = FMath::Min(MonsterClasses.Num(), SpawnPoints.Num());
	for (int32 i = 0; i < Count; i++)
	{
		if (MonsterClasses[i])
		{
			FMonsterSpawnData Data;
			Data.MonsterClass = MonsterClasses[i];
			Data.SpawnPoint = SpawnPoints[i];
			SpawnDataList.Add(Data);
		}
	}

	UE_LOG(LogMonsterAI, Log,TEXT("MonsterSpawner [%s]: Found %d SpawnPoints, %d MonsterClasses, %d matched"),*Owner->GetName(), SpawnPoints.Num(), MonsterClasses.Num(), SpawnDataList.Num());

	if (MonsterClasses.Num() != SpawnPoints.Num())
	{
		UE_LOG(LogMonsterAI, Warning,TEXT("MonsterSpawner [%s]: MonsterClasses(%d)와 SpawnPoints(%d) 수가 다릅니다"),*Owner->GetName(), MonsterClasses.Num(), SpawnPoints.Num());
	}
}
