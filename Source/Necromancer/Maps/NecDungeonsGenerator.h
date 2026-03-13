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

	// 占쏙옙占쏙옙 占쏙옙 占쏙옙占쏙옙 占쌉쇽옙
	UFUNCTION(BlueprintCallable, Category = "Room")
	void SpawnStartRoom();

	// 占쏙옙占쏙옙 占쏙옙 占쏙옙占쏙옙 占쌉쇽옙 
	UFUNCTION(BlueprintCallable, Category = "Room")
	void SpawnNextRoom();

	UFUNCTION(BlueprintCallable, Category = "Room")
	void SpawnEndRoom();


	// 占쏙옙치占쏙옙 占쏙옙 占썼열占쏙옙 占쏙옙占?

	UFUNCTION(BlueprintCallable, Category = "Room")
	void SpawnBossRoom();

	// 겹치는 방 배열에 담기
	UFUNCTION(BlueprintCallable, Category = "Overlap")
	void AddOverlappingRoomToList();

	// 占쏙옙치占쏙옙 占쏙옙 占쏙옙占쏙옙
	UFUNCTION(BlueprintCallable, Category = "Overlap")
	void CheckForOverlap();

	// 占쏙옙 占쏙옙占쏙옙 占쌉쇽옙
	UFUNCTION(BlueprintCallable, Category = "Door")
	void SpawnDoor();

	// 占쏙옙占쏙옙 占쏙옙占쏙옙
	UFUNCTION(BlueprintCallable, Category = "Wall")
	void CloseHoles();

	// 占쏙옙占쏙옙 타占싱몌옙 占쌉쇽옙
	UFUNCTION(BlueprintCallable, Category = "Timer")
	void StartDungeonTimer();

	UFUNCTION(BlueprintCallable, Category = "Timer")
	void CheckForDungeonComplete();

public:	
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	// 占쏙옙占쏙옙占쏙옙
	FTimerHandle DelayTimerHandle;
	void StartDelay();
	void OnDelayComplete();

	// 占쏙옙占쏙옙占쏙옙占쏙옙트占쏙옙 RandomArrayItemFromStream 占쏙옙占쏙옙
	TSubclassOf<AActor> RandomArrayItemFromRoom(const TArray<TSubclassOf<AActor>>& Array);

	USceneComponent* RandomArrayItemFromArrow(const TArray<USceneComponent*>& Array);

	bool GetbIsDungeonComplete() { return bIsDungeonComplete; }

	// ?앹꽦??紐⑤뱺 諛⑹쓽 以묒떖 ?꾩튂 諛섑솚 (蹂댁뒪 ?쒖같??
	UFUNCTION(BlueprintCallable, Category = "Dungeon")
	const TArray<FVector>& GetRoomLocations() const { return RoomLocations; }

protected:
	UPROPERTY(BlueprintReadWrite);
	bool bIsDungeonComplete = false;

	// 占쏙옙치占쏙옙 占쏙옙
	UPROPERTY(BlueprintReadWrite, Category = "Rooms");
	AActor* LatestRoom;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Seed");
	int32 RoomAmount = 20;

	// 占쏙옙 占쏙옙占쏙옙
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Seed");
	int32 RoomCount = 0;

	// 占쏙옙占쏙옙 占쏙옙 占쏙옙占쏙옙 占쏙옙占쏙옙
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rooms")
	TSubclassOf<AActor> StartRoom;

	// 占쏙옙占쏙옙占쏙옙 占쏙옙 占쏙옙占쏙옙 占쏙옙占쏙옙
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rooms")
	TSubclassOf<AActor> EndRoom;


	// 보스 방 담을 변수
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rooms")
	TSubclassOf<AActor> BossRoom;

	// 구멍 막기
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Holes");
	TSubclassOf<AActor> BlockHoles;

	// 占쏙옙 占썼열 占쏙옙占싱쏙옙
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rooms")
	TArray<TSubclassOf<AActor>>RoomListBase;

	//  占쏙옙 占썼열 temp
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rooms")
	TArray<TSubclassOf<AActor>>RoomList;

	// 특占쏙옙 占쏙옙 占썼열
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rooms")
	TArray<TSubclassOf<AActor>>SpecialRoomList;

	// 占쏙옙 占쏙옙치占쏙옙 Arrow 占쏙옙占쏙옙 占썼열 
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Door")
	TArray<USceneComponent*>DoorList;

	// 占쏙옙
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Door")
	TArray<TSubclassOf<AActor>>DoorActor;

	// 占쏙옙占쏙옙 占썩구 Arrow Component占쏙옙 占쏙옙占쏙옙 占썼열
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Exits")
	TArray<USceneComponent*>ExitsList;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Exits")
	TArray<USceneComponent*>SecondFExitsList;

	// 占쏙옙 占쏙옙치占쏙옙 占쏙옙占시듸옙 ArrowComponent
	USceneComponent* SelectedExitPoint;

	// 占쌘쏙옙 占쌥몌옙占쏙옙占쏙옙 占쏙옙占쏙옙 占썼열
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Overlapped")
	TArray<UPrimitiveComponent*>OverlappedList;

	// 占쏙옙占쏙옙 타占싱몌옙 占쏙옙占쏙옙
	FTimerHandle DungeonTimerHandle;

	// 占쌍댐옙 占시곤옙
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Timer")
	float MaxDungeonTime = 30.0f;

	// 占시곤옙 占쏙옙占쏙옙 占쌉쇽옙
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Timer")
	float DungeonStartTime = 0.0f;

	// 占쌕쏙옙 占쏙옙占쏙옙占쏙옙 占쏙옙占쏙옙
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
	FName LevelName = "SampleLevel";

	// ?앹꽦??紐⑤뱺 諛⑹쓽 以묒떖 ?꾩튂 (蹂댁뒪 ?쒖같??
	TArray<FVector> RoomLocations;
};
