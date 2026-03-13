// Fill out your copyright notice in the Description page of Project Settings.


#include "Maps/NecDungeonsGenerator.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"

// Sets default values
ANecDungeonsGenerator::ANecDungeonsGenerator()
{
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ANecDungeonsGenerator::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
	{
		RoomList = RoomListBase;
		RoomCount = 0;
		SpawnStartRoom();
		StartDungeonTimer();
		SpawnNextRoom();
	}
	else
	{
		return;
	}
}

TSubclassOf<AActor> ANecDungeonsGenerator::RandomArrayItemFromRoom(const TArray<TSubclassOf<AActor>>& Array)
{
	if (Array.Num() == 0)
	{
		return nullptr; // 占썩본占쏙옙 占쏙옙환
	}

	int32 OutIndex = FMath::RandRange(0, Array.Num() - 1);
	return Array[OutIndex];
}

USceneComponent* ANecDungeonsGenerator::RandomArrayItemFromArrow(const TArray<USceneComponent*>& Array)
{
	if (Array.Num() == 0)
	{
		return nullptr; // 占썩본占쏙옙 占쏙옙환
	}

	int32 OutIndex = FMath::RandRange(0, Array.Num() - 1);
	return Array[OutIndex];
}

void ANecDungeonsGenerator::SpawnStartRoom()
{
	if (HasAuthority())
	{
		FVector SpawnLocation = GetActorLocation();
		FRotator SpawnRotation = FRotator::ZeroRotator;

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		LatestRoom = GetWorld()->SpawnActor<AActor>(StartRoom, SpawnLocation, SpawnRotation, SpawnParams);

		if (LatestRoom)
		{
			// 諛??꾩튂 湲곕줉 (蹂댁뒪 ?쒖같??
			RoomLocations.Add(LatestRoom->GetActorLocation());

			// 占쏙옙占쏙옙占쏙옙 占쏙옙占쏙옙 占쏙옙占쏙옙占쏙옙트 占쏙옙占?
			TArray<USceneComponent*> Components;
			LatestRoom->GetComponents<USceneComponent>(Components);

			// 占쏙옙占쏙옙占쏙옙트占쏙옙  탐占쏙옙
			for (USceneComponent* comp : Components)
			{
				// 占쏙옙占쌩울옙 占썩구 占쏙옙占쏙옙獵占?占쏙옙 占쏙옙占쏙옙占쏙옙트
				if (comp && comp->ComponentHasTag(FName("Exits Folder")))
				{
					// 占쏙옙 占쏙옙占쏙옙占쏙옙트 占싣뤄옙 占쏙옙占쏙옙占쏙옙트占쏙옙 占썼열占쏙옙 占쏙옙占쏙옙
					const TArray<USceneComponent*>ChildCom = comp->GetAttachChildren();
					for (USceneComponent* Child : ChildCom)
					{
						// 占썩구 占쌩곤옙
						ExitsList.Add(Child);
					}
					break;
				}

			}
		}
	}
	else
	{
		return;
	}
}

void ANecDungeonsGenerator::SpawnNextRoom()
{
	if (HasAuthority())
	{
		// 占쏙옙占쏙옙 占썩구
		if (ExitsList.Num() == 0 && SecondFExitsList.Num() == 0)
		{
			return;
		}
		// 2占쏙옙 占쏙옙占쏙옙 占쏙옙占쏙옙 占쏙옙占?
		else if (SecondFExitsList.Num() != 0)
		{
			SelectedExitPoint = SecondFExitsList[0];
		}
		// 2占쏙옙 占쏙옙占쏙옙 占쏙옙占쏙옙 占쏙옙占?1占쏙옙占쏙옙占쏙옙 占쏙옙占쏙옙
		else
		{
			SelectedExitPoint = RandomArrayItemFromArrow(ExitsList);
		}

		// 占쏙옙占쏙옙占쏙옙
		TSubclassOf<AActor>NextRoom = RandomArrayItemFromRoom(RoomList);

		// 占쏙옙占쏙옙 占쏙옙치 
		FTransform SpawnTransform = SelectedExitPoint->GetComponentTransform();

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		// 占쏙옙占쏙옙 占쏙옙占쏙옙占싹곤옙 占쏙옙占쏙옙占쏙옙 占쏙옙占쏙옙
		LatestRoom = GetWorld()->SpawnActor<AActor>(NextRoom, SpawnTransform, SpawnParams);

		// 2占쏙옙 Arrow 占쏙옙占쏙옙
		SecondFExitsList.Empty();

		//占쏙옙占쏙옙占쏙옙
		StartDelay();
	}
	else
	{
		return;
	}
}

void ANecDungeonsGenerator::SpawnEndRoom()
{
	if (HasAuthority())
	{
		// 占쏙옙占쏙옙 占썩구
		if (ExitsList.Num() == 0)
		{
			return;
		}
		SelectedExitPoint = RandomArrayItemFromArrow(ExitsList);

		// 占쏙옙占쏙옙 占쏙옙치 
		FTransform SpawnTransform = SelectedExitPoint->GetComponentTransform();

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		// 占쏙옙占쏙옙 占쏙옙占쏙옙占싹곤옙 占쏙옙占쏙옙占쏙옙 占쏙옙占쏙옙
		LatestRoom = GetWorld()->SpawnActor<AActor>(EndRoom, SpawnTransform, SpawnParams);

		// 諛??꾩튂 湲곕줉 (蹂댁뒪 ?쒖같??
		if (LatestRoom)
		{
			RoomLocations.Add(LatestRoom->GetActorLocation());
		}
	}
	else
	{
		return;
	}
}

void ANecDungeonsGenerator::SpawnBossRoom()
{
	if (HasAuthority())
	{
		// 랜덤 출구
		if (ExitsList.Num() == 0)
		{
			return;
		}
		SelectedExitPoint = RandomArrayItemFromArrow(ExitsList);

		// 스폰 위치 
		FTransform SpawnTransform = SelectedExitPoint->GetComponentTransform();

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		// 액터 생성하고 변수에 저장
		LatestRoom = GetWorld()->SpawnActor<AActor>(BossRoom, SpawnTransform, SpawnParams);
	}
	else
	{
		return;
	}
}

void ANecDungeonsGenerator::AddOverlappingRoomToList()
{
	if (HasAuthority())
	{
		if (LatestRoom)
		{
			TArray<USceneComponent*> Components;
			LatestRoom->GetComponents<USceneComponent>(Components);

			for (USceneComponent* Comp : Components)
			{
				if (Comp && Comp->ComponentHasTag(FName("Overlap Folder")))
				{
					const TArray<USceneComponent*> ChildCom = Comp->GetAttachChildren();
					for (USceneComponent* Child : ChildCom)
					{
						// 占쌘식듸옙 占쌩울옙 占쌘쏙옙 占쌥몌옙占쏙옙占싱몌옙
						if (UPrimitiveComponent* PrimitiveChild = Cast<UPrimitiveComponent>(Child))
						{
							// 占쌩곤옙
							TArray<UPrimitiveComponent*> OverlappingComponents;
							PrimitiveChild->GetOverlappingComponents(OverlappingComponents);
							if (!OverlappingComponents.IsEmpty())
							{
								OverlappedList.Append(OverlappingComponents);
							}
						}
					}
					break;
				}
			}
		}
	}
	else
	{
		return;
	}
}

void ANecDungeonsGenerator::CheckForOverlap()
{
	if (HasAuthority())
	{
		AddOverlappingRoomToList();

		// 占쏙옙占쏙옙占쏙옙 占쏙옙占쏙옙트占쏙옙 占쏙옙占쏙옙占쏙옙占?占십다몌옙 占쏙옙占?占쏙옙占쏙옙占쏙옙 占쏙옙占쏙옙 占쌕몌옙 占쏙옙占?占쏙옙침 
		if (!OverlappedList.IsEmpty())
		{
			// 占쏙옙占쏙옙占쏙옙 占쏙옙占쏙옙트 占쏙옙占쏙옙占쌍곤옙, 占쏙옙 占쏙옙占쏙옙占쌍곤옙 占쌕쏙옙 占쏙옙占쏙옙
			OverlappedList.Empty();
			LatestRoom->Destroy();
			SpawnNextRoom();

			return;
		}
		// 占쏙옙占?占쏙옙占쏙옙占쏙옙 占쏙옙 占쏙옙치占쏙옙 占쏙옙占쏙옙
		else
		{
			OverlappedList.Empty();
			RoomCount += 1;

			// 諛??꾩튂 湲곕줉 (蹂댁뒪 ?쒖같??
			if (LatestRoom)
			{
				RoomLocations.Add(LatestRoom->GetActorLocation());
			}

			ExitsList.Remove(SelectedExitPoint);
			//SelectedExitPoint = nullptr;
			if (LatestRoom)
			{
				TArray<USceneComponent*> Components;
				LatestRoom->GetComponents<USceneComponent>(Components);

				// 占쏙옙占쏙옙占쏙옙트占쏙옙  탐占쏙옙
				for (USceneComponent* comp : Components)
				{
					// 占쏙옙占쌩울옙 占썩구 占쏙옙占쏙옙獵占?占쏙옙 占쏙옙占쏙옙占쏙옙트
					if (comp && comp->ComponentHasTag(FName("2F Exits Folder")))
					{
						const TArray<USceneComponent*>ChildCom = comp->GetAttachChildren();
						for (USceneComponent* Child : ChildCom)
						{
							// 占썩구 占쌩곤옙
							SecondFExitsList.Add(Child);
						}
						break;
					}
					else if (comp && comp->ComponentHasTag(FName("Exits Folder")))
					{
						// 占쏙옙 占쏙옙占쏙옙占쏙옙트 占싣뤄옙 占쏙옙占쏙옙占쏙옙트占쏙옙 占썼열占쏙옙 占쏙옙占쏙옙
						const TArray<USceneComponent*>ChildCom = comp->GetAttachChildren();
						for (USceneComponent* Child : ChildCom)
						{
							// 占썩구 占쌩곤옙
							ExitsList.Add(Child);
						}
						break;
					}
				}
			}

			// 占쏙옙 占쏙옙치占쏙옙 占쏙옙 占쏙옙占쏙옙트占쏙옙 占쏙옙占?
			DoorList.Add(SelectedExitPoint);
		}

		// 占쏙옙占쏙옙 占쏙옙 占쏙옙치 占쏙옙占쏙옙占싹몌옙 占썸설치
		if (RoomCount < RoomAmount)
		{
			// 占쏙옙 10占쏙옙 占쏙옙占쏙옙占쏙옙 占쏙옙占쏙옙占쏙옙 특占쏙옙占쏙옙 占쏙옙 占쌩곤옙
			if (RoomCount % 10 == 0)
			{
				RoomList = SpecialRoomList;
			}
			else
			{
				RoomList = RoomListBase;
			}
			SpawnNextRoom();
		}
		else
		{


			// 占쏙옙占쏙옙占쏙옙 占쏙옙 占쏙옙占쏙옙
			// 보스 방 생성
			SpawnBossRoom();
			// 마지막 방 생성
			SpawnEndRoom();
			// 占쏙옙占쏙옙 占쏙옙占쏙옙
			CloseHoles();
			// 占쏙옙 占쏙옙占쏙옙
			SpawnDoor();
			// 타占싱몌옙 占쏙옙占쏙옙
			GetWorld()->GetTimerManager().ClearTimer(DungeonTimerHandle);


			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, TEXT("Dungeon Complete"));
			// 占쏙옙占쏙옙 占쏙옙占쏙옙 占싹뤄옙
			bIsDungeonComplete = true;
		}
	}
	else
	{
		return;
	}
}

void ANecDungeonsGenerator::CloseHoles()
{
	if (ExitsList.Num() > 0)
	{
		// 占쏙옙占쏙옙占쏙옙 占썸돌 占쏙옙占시뤄옙
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		for (USceneComponent* comp : ExitsList)
		{
			FTransform Transform = comp->GetComponentTransform();
			GetWorld()->SpawnActor<AActor>(BlockHoles, Transform, SpawnParams);
		}
	}
}

void ANecDungeonsGenerator::SpawnDoor()
{
	if (DoorList.Num() > 0)
	{
		for (USceneComponent* DoorPoint : DoorList)
		{
			TSubclassOf<AActor> Door = RandomArrayItemFromRoom(DoorActor);
			FTransform Transform = DoorPoint->GetComponentTransform();
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			GetWorld()->SpawnActor<AActor>(Door, Transform, SpawnParams);
		}
	}
}

void ANecDungeonsGenerator::StartDungeonTimer()
{
	// 占쏙옙占쏙옙 占시곤옙 占쏙옙占?
	DungeonStartTime = GetWorld()->GetTimeSeconds();

	// 占쏙옙占쏙옙占쏙옙占쏙옙트占쏙옙 占쏙옙占쏙옙占싹깍옙 占쏙옙占쏙옙 타占싱몌옙
	GetWorld()->GetTimerManager().SetTimer(
		DungeonTimerHandle,
		this,
		&ANecDungeonsGenerator::CheckForDungeonComplete,
		1.0f, // 1占십몌옙占쏙옙 체크
		true // 占쏙옙占쏙옙
	);
}

void ANecDungeonsGenerator::CheckForDungeonComplete()
{
	// 占쏙옙占쏙옙챨占?占쏙옙占?
	float RunningTime = GetWorld()->GetTimeSeconds() - DungeonStartTime;

	if (RunningTime >= MaxDungeonTime)
	{
		// 占쏙옙占쏙옙 占쏙옙占쏙옙占?
		UGameplayStatics::OpenLevel(this, LevelName);
	}
}

void ANecDungeonsGenerator::StartDelay()
{
	GetWorld()->GetTimerManager().SetTimer(DelayTimerHandle, this, &ANecDungeonsGenerator::OnDelayComplete,
		0.01f, false);
}

void ANecDungeonsGenerator::OnDelayComplete()
{
	CheckForOverlap();
	UE_LOG(LogTemp, Log, TEXT("占쏙옙占쏙옙占쏙옙 占쏙옙"));
}