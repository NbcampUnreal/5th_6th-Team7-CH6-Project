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
		return nullptr; // 기본값 반환
	}

	int32 OutIndex = FMath::RandRange(0, Array.Num() - 1);
	return Array[OutIndex];
}

USceneComponent* ANecDungeonsGenerator::RandomArrayItemFromArrow(const TArray<USceneComponent*>& Array)
{
	if (Array.Num() == 0)
	{
		return nullptr; // 기본값 반환
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
			// 생성한 방의 컴포넌트 담기
			TArray<USceneComponent*> Components;
			LatestRoom->GetComponents<USceneComponent>(Components);

			// 컴포넌트들  탐색
			for (USceneComponent* comp : Components)
			{
				// 그중에 출구 들어있는 씬 컴포넌트
				if (comp && comp->ComponentHasTag(FName("Exits Folder")))
				{
					// 신 컴포넌트 아래 컴포넌트들 배열에 저장
					const TArray<USceneComponent*>ChildCom = comp->GetAttachChildren();
					for (USceneComponent* Child : ChildCom)
					{
						// 출구 추가
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
		// 랜덤 출구
		if (ExitsList.Num() == 0 && SecondFExitsList.Num() == 0)
		{
			return;
		}
		// 2층 방이 있을 경우
		else if (SecondFExitsList.Num() != 0)
		{
			SelectedExitPoint = SecondFExitsList[0];
		}
		// 2층 방이 없는 경우 1층에서 랜덤
		else
		{
			SelectedExitPoint = RandomArrayItemFromArrow(ExitsList);
		}

		// 랜덤방
		TSubclassOf<AActor>NextRoom = RandomArrayItemFromRoom(RoomList);

		// 스폰 위치 
		FTransform SpawnTransform = SelectedExitPoint->GetComponentTransform();

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		// 액터 생성하고 변수에 저장
		LatestRoom = GetWorld()->SpawnActor<AActor>(NextRoom, SpawnTransform, SpawnParams);

		// 2층 Arrow 삭제
		SecondFExitsList.Empty();

		//딜레이
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
		LatestRoom = GetWorld()->SpawnActor<AActor>(EndRoom, SpawnTransform, SpawnParams);
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
						// 자식들 중에 박스 콜리전이면
						if (UPrimitiveComponent* PrimitiveChild = Cast<UPrimitiveComponent>(Child))
						{
							// 추가
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

		// 오버랩 리스트가 비어있지 않다면 방금 생성한 방이 다른 방과 겹침 
		if (!OverlappedList.IsEmpty())
		{
			// 오버랩 리스트 지워주고, 방 지워주고 다시 생성
			OverlappedList.Empty();
			LatestRoom->Destroy();
			SpawnNextRoom();

			return;
		}
		// 방금 생성한 방 겹치지 않음
		else
		{
			OverlappedList.Empty();
			RoomCount += 1;
			ExitsList.Remove(SelectedExitPoint);
			//SelectedExitPoint = nullptr;
			if (LatestRoom)
			{
				TArray<USceneComponent*> Components;
				LatestRoom->GetComponents<USceneComponent>(Components);

				// 컴포넌트들  탐색
				for (USceneComponent* comp : Components)
				{
					// 그중에 출구 들어있는 씬 컴포넌트
					if (comp && comp->ComponentHasTag(FName("2F Exits Folder")))
					{
						const TArray<USceneComponent*>ChildCom = comp->GetAttachChildren();
						for (USceneComponent* Child : ChildCom)
						{
							// 출구 추가
							SecondFExitsList.Add(Child);
						}
						break;
					}
					else if (comp && comp->ComponentHasTag(FName("Exits Folder")))
					{
						// 신 컴포넌트 아래 컴포넌트들 배열에 저장
						const TArray<USceneComponent*>ChildCom = comp->GetAttachChildren();
						for (USceneComponent* Child : ChildCom)
						{
							// 출구 추가
							ExitsList.Add(Child);
						}
						break;
					}
				}
			}

			// 문 설치할 곳 리스트에 담기
			DoorList.Add(SelectedExitPoint);
		}

		// 아직 방 설치 가능하면 방설치
		if (RoomCount < RoomAmount)
		{
			// 방 10번 생성될 때마다 특수한 방 추가
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
			// 보스 방 생성
			SpawnBossRoom();
			// 마지막 방 생성
			SpawnEndRoom();
			// 구멍 막기
			CloseHoles();
			// 문 생성
			SpawnDoor();
			// 타이머 종료
			GetWorld()->GetTimerManager().ClearTimer(DungeonTimerHandle);


			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, TEXT("Dungeon Complete"));
			// 던전 생성 완료
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
		// 스폰시 충돌 무시로
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
	// 시작 시간 기록
	DungeonStartTime = GetWorld()->GetTimeSeconds();

	// 델리게이트를 실행하기 위한 타이머
	GetWorld()->GetTimerManager().SetTimer(
		DungeonTimerHandle,
		this,
		&ANecDungeonsGenerator::CheckForDungeonComplete,
		1.0f, // 1초마다 체크
		true // 루프
	);
}

void ANecDungeonsGenerator::CheckForDungeonComplete()
{
	// 경과시간 계산
	float RunningTime = GetWorld()->GetTimeSeconds() - DungeonStartTime;

	if (RunningTime >= MaxDungeonTime)
	{
		// 레벨 재시작
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