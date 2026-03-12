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
		return nullptr; // �⺻�� ��ȯ
	}

	int32 OutIndex = FMath::RandRange(0, Array.Num() - 1);
	return Array[OutIndex];
}

USceneComponent* ANecDungeonsGenerator::RandomArrayItemFromArrow(const TArray<USceneComponent*>& Array)
{
	if (Array.Num() == 0)
	{
		return nullptr; // �⺻�� ��ȯ
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
			// 방 위치 기록 (보스 순찰용)
			RoomLocations.Add(LatestRoom->GetActorLocation());

			// ������ ���� ������Ʈ ���
			TArray<USceneComponent*> Components;
			LatestRoom->GetComponents<USceneComponent>(Components);

			// ������Ʈ��  Ž��
			for (USceneComponent* comp : Components)
			{
				// ���߿� �ⱸ ����ִ� �� ������Ʈ
				if (comp && comp->ComponentHasTag(FName("Exits Folder")))
				{
					// �� ������Ʈ �Ʒ� ������Ʈ�� �迭�� ����
					const TArray<USceneComponent*>ChildCom = comp->GetAttachChildren();
					for (USceneComponent* Child : ChildCom)
					{
						// �ⱸ �߰�
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
		// ���� �ⱸ
		if (ExitsList.Num() == 0 && SecondFExitsList.Num() == 0)
		{
			return;
		}
		// 2�� ���� ���� ���
		else if (SecondFExitsList.Num() != 0)
		{
			SelectedExitPoint = SecondFExitsList[0];
		}
		// 2�� ���� ���� ��� 1������ ����
		else
		{
			SelectedExitPoint = RandomArrayItemFromArrow(ExitsList);
		}

		// ������
		TSubclassOf<AActor>NextRoom = RandomArrayItemFromRoom(RoomList);

		// ���� ��ġ 
		FTransform SpawnTransform = SelectedExitPoint->GetComponentTransform();

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		// ���� �����ϰ� ������ ����
		LatestRoom = GetWorld()->SpawnActor<AActor>(NextRoom, SpawnTransform, SpawnParams);

		// 2�� Arrow ����
		SecondFExitsList.Empty();

		//������
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
		// ���� �ⱸ
		if (ExitsList.Num() == 0)
		{
			return;
		}
		SelectedExitPoint = RandomArrayItemFromArrow(ExitsList);

		// ���� ��ġ 
		FTransform SpawnTransform = SelectedExitPoint->GetComponentTransform();

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		// ���� �����ϰ� ������ ����
		LatestRoom = GetWorld()->SpawnActor<AActor>(EndRoom, SpawnTransform, SpawnParams);

		// 방 위치 기록 (보스 순찰용)
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
						// �ڽĵ� �߿� �ڽ� �ݸ����̸�
						if (UPrimitiveComponent* PrimitiveChild = Cast<UPrimitiveComponent>(Child))
						{
							// �߰�
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

		// ������ ����Ʈ�� ������� �ʴٸ� ��� ������ ���� �ٸ� ��� ��ħ 
		if (!OverlappedList.IsEmpty())
		{
			// ������ ����Ʈ �����ְ�, �� �����ְ� �ٽ� ����
			OverlappedList.Empty();
			LatestRoom->Destroy();
			SpawnNextRoom();

			return;
		}
		// ��� ������ �� ��ġ�� ����
		else
		{
			OverlappedList.Empty();
			RoomCount += 1;

			// 방 위치 기록 (보스 순찰용)
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

				// ������Ʈ��  Ž��
				for (USceneComponent* comp : Components)
				{
					// ���߿� �ⱸ ����ִ� �� ������Ʈ
					if (comp && comp->ComponentHasTag(FName("2F Exits Folder")))
					{
						const TArray<USceneComponent*>ChildCom = comp->GetAttachChildren();
						for (USceneComponent* Child : ChildCom)
						{
							// �ⱸ �߰�
							SecondFExitsList.Add(Child);
						}
						break;
					}
					else if (comp && comp->ComponentHasTag(FName("Exits Folder")))
					{
						// �� ������Ʈ �Ʒ� ������Ʈ�� �迭�� ����
						const TArray<USceneComponent*>ChildCom = comp->GetAttachChildren();
						for (USceneComponent* Child : ChildCom)
						{
							// �ⱸ �߰�
							ExitsList.Add(Child);
						}
						break;
					}
				}
			}

			// �� ��ġ�� �� ����Ʈ�� ���
			DoorList.Add(SelectedExitPoint);
		}

		// ���� �� ��ġ �����ϸ� �漳ġ
		if (RoomCount < RoomAmount)
		{
			// �� 10�� ������ ������ Ư���� �� �߰�
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


			// ������ �� ����
			SpawnEndRoom();
			// ���� ����
			CloseHoles();
			// �� ����
			SpawnDoor();
			// Ÿ�̸� ����
			GetWorld()->GetTimerManager().ClearTimer(DungeonTimerHandle);


			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, TEXT("Dungeon Complete"));
			// ���� ���� �Ϸ�
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
		// ������ �浹 ���÷�
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
	// ���� �ð� ���
	DungeonStartTime = GetWorld()->GetTimeSeconds();

	// ��������Ʈ�� �����ϱ� ���� Ÿ�̸�
	GetWorld()->GetTimerManager().SetTimer(
		DungeonTimerHandle,
		this,
		&ANecDungeonsGenerator::CheckForDungeonComplete,
		1.0f, // 1�ʸ��� üũ
		true // ����
	);
}

void ANecDungeonsGenerator::CheckForDungeonComplete()
{
	// ����ð� ���
	float RunningTime = GetWorld()->GetTimeSeconds() - DungeonStartTime;

	if (RunningTime >= MaxDungeonTime)
	{
		// ���� �����
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
	UE_LOG(LogTemp, Log, TEXT("������ ��"));
}