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
		SpecialCount = 0;
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
		return nullptr;
	}

	int32 OutIndex = FMath::RandRange(0, Array.Num() - 1);
	return Array[OutIndex];
}

USceneComponent* ANecDungeonsGenerator::RandomArrayItemFromArrow(const TArray<USceneComponent*>& Array)
{
	if (Array.Num() == 0)
	{
		return nullptr;
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
			RoomLocations.Add(LatestRoom->GetActorLocation());

			TArray<USceneComponent*> Components;
			LatestRoom->GetComponents<USceneComponent>(Components);

			for (USceneComponent* comp : Components)
			{
				if (comp && comp->ComponentHasTag(FName("Exits Folder")))
				{
					const TArray<USceneComponent*>ChildCom = comp->GetAttachChildren();
					for (USceneComponent* Child : ChildCom)
					{
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
		if (ExitsList.Num() == 0 && SecondFExitsList.Num() == 0)
		{
			return;
		}
		else if (SecondFExitsList.Num() != 0)
		{
			SelectedExitPoint = SecondFExitsList[0];
		}
		else
		{
			SelectedExitPoint = RandomArrayItemFromArrow(ExitsList);
		}

		TSubclassOf<AActor>NextRoom = nullptr;

		if (bIsSpecialList)
		{
			// SpecialRoomList 소진 시 RoomListBase로 복귀
			if (!RoomList.IsValidIndex(SpecialCount))
			{
				RoomList = RoomListBase;
				SpecialCount = 0;
				NextRoom = RandomArrayItemFromRoom(RoomList);
			}
			else
			{
				NextRoom = RoomList[SpecialCount];
				SpecialCount++;
			}
		}
		else
		{
			NextRoom = RandomArrayItemFromRoom(RoomList);
		}

		FTransform SpawnTransform = SelectedExitPoint->GetComponentTransform();

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		LatestRoom = GetWorld()->SpawnActor<AActor>(NextRoom, SpawnTransform, SpawnParams);

		SecondFExitsList.Empty();

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
		if (ExitsList.Num() == 0)
		{
			return;
		}
		SelectedExitPoint = RandomArrayItemFromArrow(ExitsList);

		FTransform SpawnTransform = SelectedExitPoint->GetComponentTransform();

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		LatestRoom = GetWorld()->SpawnActor<AActor>(EndRoom, SpawnTransform, SpawnParams);

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
						if (UPrimitiveComponent* PrimitiveChild = Cast<UPrimitiveComponent>(Child))
						{
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

		if (!OverlappedList.IsEmpty())
		{
			OverlappedList.Empty();
			LatestRoom->Destroy();

			// 스페셜 방이 거부됐으면 카운트 되돌리기
			if (RoomList == SpecialRoomList && SpecialCount > 0)
			{
				SpecialCount--;
			}

			SpawnNextRoom();

			return;
		}
		else
		{
			OverlappedList.Empty();
			RoomCount += 1;

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

				for (USceneComponent* comp : Components)
				{
					if (comp && comp->ComponentHasTag(FName("2F Exits Folder")))
					{
						const TArray<USceneComponent*>ChildCom = comp->GetAttachChildren();
						for (USceneComponent* Child : ChildCom)
						{
							SecondFExitsList.Add(Child);
						}
						break;
					}
					else if (comp && comp->ComponentHasTag(FName("Exits Folder")))
					{
						const TArray<USceneComponent*>ChildCom = comp->GetAttachChildren();
						for (USceneComponent* Child : ChildCom)
						{
							ExitsList.Add(Child);
						}
						break;
					}
				}
			}

			DoorList.Add(SelectedExitPoint);
		}

		if (RoomCount < RoomAmount)
		{
			if (RoomCount % ((RoomAmount / SpecialRoomList.Num()) - 1) == 0)
			{
				RoomList = SpecialRoomList;
				bIsSpecialList = true;
			}
			else
			{
				RoomList = RoomListBase;
				bIsSpecialList = false;
			}
			SpawnNextRoom();
		}
		else
		{
			// 마지막 방 생성
			SpawnEndRoom();
			// 구멍 막기
			CloseHoles();
			// 문 생성
			SpawnDoor();

			GetWorld()->GetTimerManager().ClearTimer(DungeonTimerHandle);


			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, TEXT("Dungeon Complete"));
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
	DungeonStartTime = GetWorld()->GetTimeSeconds();

	GetWorld()->GetTimerManager().SetTimer(
		DungeonTimerHandle,
		this,
		&ANecDungeonsGenerator::CheckForDungeonComplete,
		1.0f,
		true
	);
}

void ANecDungeonsGenerator::CheckForDungeonComplete()
{
	float RunningTime = GetWorld()->GetTimeSeconds() - DungeonStartTime;

	if (RunningTime >= MaxDungeonTime)
	{
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
	UE_LOG(LogTemp, Log, TEXT("딜레이 끝"));
}