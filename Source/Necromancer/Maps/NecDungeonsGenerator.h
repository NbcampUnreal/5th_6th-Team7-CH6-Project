// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NecDungeonsGenerator.generated.h"

UCLASS()
class NECROMANCER_API ANecDungeonsGenerator : public AActor
{
	GENERATED_BODY()
	
public:
	ANecDungeonsGenerator();

protected:
	virtual void BeginPlay() override;

	// ���� �� ���� �Լ�
	UFUNCTION(BlueprintCallable, Category = "Room")
	void SpawnStartRoom();

	// ���� �� ���� �Լ� 
	UFUNCTION(BlueprintCallable, Category = "Room")
	void SpawnNextRoom();

	UFUNCTION(BlueprintCallable, Category = "Room")
	void SpawnEndRoom();

<<<<<<< HEAD
	// ��ġ�� �� �迭�� ���
=======
	UFUNCTION(BlueprintCallable, Category = "Room")
	void SpawnBossRoom();

	// ��ġ�� �� �迭�� ���
>>>>>>> LDG
	UFUNCTION(BlueprintCallable, Category = "Overlap")
	void AddOverlappingRoomToList();

	// ��ġ�� �� ����
	UFUNCTION(BlueprintCallable, Category = "Overlap")
	void CheckForOverlap();

	// �� ���� �Լ�
	UFUNCTION(BlueprintCallable, Category = "Door")
	void SpawnDoor();

	// ���� ����
	UFUNCTION(BlueprintCallable, Category = "Wall")
	void CloseHoles();

	// ���� Ÿ�̸� �Լ�
	UFUNCTION(BlueprintCallable, Category = "Timer")
	void StartDungeonTimer();

	UFUNCTION(BlueprintCallable, Category = "Timer")
	void CheckForDungeonComplete();

public:	
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	// ������
	FTimerHandle DelayTimerHandle;
	void StartDelay();
	void OnDelayComplete();

	// ��������Ʈ�� RandomArrayItemFromStream ����
	TSubclassOf<AActor> RandomArrayItemFromRoom(const TArray<TSubclassOf<AActor>>& Array);

	USceneComponent* RandomArrayItemFromArrow(const TArray<USceneComponent*>& Array);

	bool GetbIsDungeonComplete() { return bIsDungeonComplete; }

	// 생성된 모든 방의 중심 위치 반환 (보스 순찰용)
	UFUNCTION(BlueprintCallable, Category = "Dungeon")
	const TArray<FVector>& GetRoomLocations() const { return RoomLocations; }

protected:
	UPROPERTY(BlueprintReadWrite);
	bool bIsDungeonComplete = false;

	// ��ġ�� ��
	UPROPERTY(BlueprintReadWrite, Category = "Rooms");
	AActor* LatestRoom;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Seed");
	int32 RoomAmount = 20;

	// �� ����
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Seed");
	int32 RoomCount = 0;

	// ���� �� ���� ����
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rooms")
	TSubclassOf<AActor> StartRoom;

	// ������ �� ���� ����
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rooms")
	TSubclassOf<AActor> EndRoom;

<<<<<<< HEAD
	// ���� ����
=======
	// ���� �� ���� ����
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rooms")
	TSubclassOf<AActor> BossRoom;

	// ���� ����
>>>>>>> LDG
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Holes");
	TSubclassOf<AActor> BlockHoles;

	// �� �迭 ���̽�
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rooms")
	TArray<TSubclassOf<AActor>>RoomListBase;

	//  �� �迭 temp
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rooms")
	TArray<TSubclassOf<AActor>>RoomList;

	// Ư�� �� �迭
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rooms")
	TArray<TSubclassOf<AActor>>SpecialRoomList;

	// �� ��ġ�� Arrow ���� �迭 
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Door")
	TArray<USceneComponent*>DoorList;

	// ��
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Door")
	TArray<TSubclassOf<AActor>>DoorActor;

	// ���� �ⱸ Arrow Component�� ���� �迭
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Exits")
	TArray<USceneComponent*>ExitsList;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Exits")
	TArray<USceneComponent*>SecondFExitsList;

	// �� ��ġ�� ���õ� ArrowComponent
	USceneComponent* SelectedExitPoint;

	// �ڽ� �ݸ����� ���� �迭
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Overlapped")
	TArray<UPrimitiveComponent*>OverlappedList;

	// ���� Ÿ�̸� ����
	FTimerHandle DungeonTimerHandle;

	// �ִ� �ð�
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Timer")
	float MaxDungeonTime = 30.0f;

	// �ð� ���� �Լ�
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Timer")
	float DungeonStartTime = 0.0f;

	// �ٽ� ������ ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
	FName LevelName = "SampleLevel";

	// 생성된 모든 방의 중심 위치 (보스 순찰용)
	TArray<FVector> RoomLocations;
};
