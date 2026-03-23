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

	UFUNCTION(BlueprintCallable, Category = "Room")
	void SpawnStartRoom();

	UFUNCTION(BlueprintCallable, Category = "Room")
	void SpawnNextRoom();

	UFUNCTION(BlueprintCallable, Category = "Room")
	void SpawnEndRoom();

	// 겹치는 방 배열에 담기
	UFUNCTION(BlueprintCallable, Category = "Overlap")
	void AddOverlappingRoomToList();

	UFUNCTION(BlueprintCallable, Category = "Overlap")
	void CheckForOverlap();

	UFUNCTION(BlueprintCallable, Category = "Door")
	void SpawnDoor();

	UFUNCTION(BlueprintCallable, Category = "Wall")
	void CloseHoles();

	UFUNCTION(BlueprintCallable, Category = "Timer")
	void StartDungeonTimer();

	UFUNCTION(BlueprintCallable, Category = "Timer")
	void CheckForDungeonComplete();

public:	
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	FTimerHandle DelayTimerHandle;
	void StartDelay();
	void OnDelayComplete();

	TSubclassOf<AActor> RandomArrayItemFromRoom(const TArray<TSubclassOf<AActor>>& Array);

	USceneComponent* RandomArrayItemFromArrow(const TArray<USceneComponent*>& Array);

	bool GetbIsDungeonComplete() { return bIsDungeonComplete; }

	UFUNCTION(BlueprintCallable, Category = "Dungeon")
	const TArray<FVector>& GetRoomLocations() const { return RoomLocations; }

protected:
	UPROPERTY(BlueprintReadWrite);
	bool bIsDungeonComplete = false;

	UPROPERTY(BlueprintReadWrite, Category = "Rooms");
	AActor* LatestRoom;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Seed");
	int32 RoomAmount = 20;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Seed");
	int32 RoomCount = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Seed");
	int32 SpecialCount = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rooms")
	TSubclassOf<AActor> StartRoom;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rooms")
	TSubclassOf<AActor> EndRoom;


	// 보스 방 담을 변수
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rooms")
	TSubclassOf<AActor> BossRoom;

	// 구멍 막기
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Holes");
	TSubclassOf<AActor> BlockHoles;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rooms")
	TArray<TSubclassOf<AActor>>RoomListBase;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rooms")
	TArray<TSubclassOf<AActor>>RoomList;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rooms")
	TArray<TSubclassOf<AActor>>SpecialRoomList;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Door")
	TArray<USceneComponent*>DoorList;

	// 占쏙옙
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Door")
	TArray<TSubclassOf<AActor>>DoorActor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Exits")
	TArray<USceneComponent*>ExitsList;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Exits")
	TArray<USceneComponent*>SecondFExitsList;

	USceneComponent* SelectedExitPoint;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Overlapped")
	TArray<UPrimitiveComponent*>OverlappedList;

	FTimerHandle DungeonTimerHandle;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Timer")
	float MaxDungeonTime = 30.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Timer")
	float DungeonStartTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
	FName LevelName = "InGameDungeon";

	TArray<FVector> RoomLocations;
};
