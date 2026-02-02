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

	// 시작 방 생성 함수
	UFUNCTION(BlueprintCallable, Category = "Room")
	void SpawnStartRoom();

	// 다음 방 생성 함수
	UFUNCTION(BlueprintCallable, Category = "Room")
	void SpawnNextRoom();

	// 겹치는 방 배열에 담기
	UFUNCTION(BlueprintCallable, Category = "Overlap")
	void AddOverlappingRoomToList();

	// 겹치는 방 삭제
	UFUNCTION(BlueprintCallable, Category = "Overlap")
	void CheckForOverlap();

	// 구멍 막기
	UFUNCTION(BlueprintCallable, Category = "Wall")
	void CloseHoles();

	// 던전 타이머 함수
	UFUNCTION(BlueprintCallable, Category = "Timer")
	void StartDungeonTimer();

	UFUNCTION(BlueprintCallable, Category = "Timer")
	void CheckForDungeonComplete();

public:	
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	// 딜레이
	FTimerHandle DelayTimerHandle;
	void StartDelay();
	void OnDelayComplete();

	// 블루프린트의 RandomArrayItemFromStream 구현
	TSubclassOf<AActor> RandomArrayItemFromStreamRoom(const TArray<TSubclassOf<AActor>>& Array);

	USceneComponent* RandomArrayItemFromStreamArrow(const TArray<USceneComponent*>& Array);

	bool GetbIsDungeonComplete() { return bIsDungeonComplete; }

protected:
	UPROPERTY(BlueprintReadWrite);
	bool bIsDungeonComplete = false;

	// 설치된 방
	UPROPERTY(BlueprintReadWrite, Category = "Rooms");
	AActor* LatestRoom;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Seed");
	int32 RoomAmount = 20;

	// 방 개수
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Seed");
	int32 RoomCount = 0;

	// 시작 방 담을 변수
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rooms")
	TSubclassOf<AActor> StartRoom;

	// 구멍 막기
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Holes");
	TSubclassOf<AActor> BlockHoles;

	// 방 배열 베이스
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rooms")
	TArray<TSubclassOf<AActor>>RoomListBase;


	//  방 배열 temp
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rooms")
	TArray<TSubclassOf<AActor>>RoomList;

	// 특수 방 배열
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rooms")
	TArray<TSubclassOf<AActor>>SpecialRoomList;

	// 방의 출구 Arrow Component를 담을 배열
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Exits")
	TArray<USceneComponent*>ExitsList;

	// 방 설치에 선택된 ArrowComponent
	USceneComponent* SelectedExitPoint;

	// 박스 콜리전을 담을 배열
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Overlapped")
	TArray<UPrimitiveComponent*>OverlappedList;

	// 던전 타이머 관련
	FTimerHandle DungeonTimerHandle;

	// 최대 시간
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Timer")
	float MaxDungeonTime = 30.0f;

	// 시간 세는 함수
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Timer")
	float DungeonStartTime = 0.0f;

	// 다시 시작할 레벨
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
	FName LevelName = "SampleLevel";
};
