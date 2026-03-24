// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "NecSaveGameSubsystem.generated.h"


class UNecProfileSaveGame;
class UNecSessionSaveGame;

/**
 * 
 */
UCLASS()
class NECROMANCER_API UNecSaveGameSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;


#pragma region ProfileSaveGame
private:
	UPROPERTY()
	UNecProfileSaveGame* ProfileSaveGame;

	const FString ProfileSaveGameSlotName = FString(TEXT("Profile"));
	
private:
	void InitProfileSaveGame();
	void SaveProfileSaveGame();

public:
	void IncreaseProfileKillCount();
	int32 GetProfileKillCount();

#pragma endregion

#pragma region SessionSaveGame
private:
	UPROPERTY()
	UNecSessionSaveGame* SessionSaveGame;

	const FString DefaultSessionSaveGameSlotName = FString(TEXT("Session"));

public:
	/// <summary>
	/// -1: NewGame (SlotName = Session) -> If Player doesn't save, this SessionSaveGame is gone)
	/// 0~2: Saved Game (SlotName = FString::Printf("Session_%d", SaveIdx))
	/// </summary>
	/// 
	/// <param name="SlotIdx">
	/// if SlotIdx == -1 then SaveCurrentSession, else then SaveGameSession to Continue Game
	/// </param>
	void InitSessionSaveGame(int32 SlotIdx = -1);
	void SaveSessionSaveGame(int32 SlotIdx = -1);

	void SaveSessionData();

public:
	int32 GetLvDepth() const;
	int32 GetSubmittedItemValue() const;
	int32 GetKillCount() const;


#pragma endregion
};
