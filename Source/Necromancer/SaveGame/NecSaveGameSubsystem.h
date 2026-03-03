// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "NecSaveGameSubsystem.generated.h"


class UNecProfileSaveGame;
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
public:

	UPROPERTY()
	UNecProfileSaveGame* ProfileSaveGame;

	const FString ProfileSaveGameSlotName = FString(TEXT("Profile"));
	
private:
	void InitProfileSaveGame();

public:
	UFUNCTION(BlueprintCallable)
	void IncreaseKillCount();

private:
	UFUNCTION(BlueprintCallable)
	void SaveProfileSaveGame();

#pragma endregion
};
