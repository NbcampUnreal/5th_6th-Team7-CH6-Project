// Fill out your copyright notice in the Description page of Project Settings.

#include "NecSaveGameSubsystem.h"
#include "SaveGame/NecSaveGameSubsystem.h"

#include "Kismet/GameplayStatics.h"

#include "SaveGame/NecProfileSaveGame.h"
#include "SaveGame/NecSessionSaveGame.h"

void UNecSaveGameSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	InitProfileSaveGame();
}

void UNecSaveGameSubsystem::InitProfileSaveGame()
{
    if (UGameplayStatics::DoesSaveGameExist(ProfileSaveGameSlotName, 0))
    {
        ProfileSaveGame = Cast<UNecProfileSaveGame>(UGameplayStatics::LoadGameFromSlot(ProfileSaveGameSlotName, 0));
    }
    else
    {
        ProfileSaveGame = Cast<UNecProfileSaveGame>(UGameplayStatics::CreateSaveGameObject(UNecProfileSaveGame::StaticClass()));
        UGameplayStatics::SaveGameToSlot(ProfileSaveGame, ProfileSaveGameSlotName, 0);
    }
}

void UNecSaveGameSubsystem::SaveProfileSaveGame()
{
    UGameplayStatics::SaveGameToSlot(ProfileSaveGame, ProfileSaveGameSlotName, 0);
}

void UNecSaveGameSubsystem::IncreaseProfileKillCount()
{
    ProfileSaveGame->TotalKillCount++;
    //UE_LOG(LogTemp, Warning, TEXT("Kill Count Updated! Total: %d"), ProfileSaveGame ? ProfileSaveGame->TotalKillCount : 0);

    //SaveProfileSaveGame();
}

int32 UNecSaveGameSubsystem::GetProfileKillCount()
{
    return ProfileSaveGame->TotalKillCount;
}

void UNecSaveGameSubsystem::InitSessionSaveGame(int32 SlotIdx = -1)
{
    // New Game
    if (SlotIdx == -1) 
    {
        SessionSaveGame = Cast<UNecSessionSaveGame>(UGameplayStatics::CreateSaveGameObject(UNecSessionSaveGame::StaticClass()));
        UGameplayStatics::SaveGameToSlot(SessionSaveGame, DefaultSessionSaveGameSlotName, 0);
    }
    else
    {
        // Load Game
        FString SessionSlotName = FString::Printf(TEXT("Session_%d"), SlotIdx);
        SessionSaveGame = Cast<UNecSessionSaveGame>(UGameplayStatics::LoadGameFromSlot(SessionSlotName, 0));
    }
}

void UNecSaveGameSubsystem::SaveSessionSaveGame(int32 SlotIdx = -1)
{
    if (SlotIdx == -1)
    {
        UGameplayStatics::SaveGameToSlot(SessionSaveGame, DefaultSessionSaveGameSlotName, 0);
    }
    else
    {
        FString SessionSaveGameSlotName = FString::Printf(TEXT("Session_%d"), SlotIdx);
        UGameplayStatics::SaveGameToSlot(SessionSaveGame, SessionSaveGameSlotName, 0);
    }
}

void UNecSaveGameSubsystem::IncreaseLvDepth()
{
    SessionSaveGame->LvDepth++;
}

int32 UNecSaveGameSubsystem::GetLvDepth()
{
    return SessionSaveGame->LvDepth;
}

void UNecSaveGameSubsystem::IncreaseKillCount()
{
    SessionSaveGame->KillCount++;
}

int32 UNecSaveGameSubsystem::GetKillCount() const
{
    return SessionSaveGame->KillCount;
}

float UNecSaveGameSubsystem::GetSpawnCostMultiplier() const
{
    if (!SessionSaveGame) return 1.0f;

    int32 Level = SessionSaveGame->LvDepth;

    return 1.0f + (Level * 0.2f);
}

int32 UNecSaveGameSubsystem::GetLevelMaxSpawnCost() const
{
    if (!SessionSaveGame) return 100;

    int32 Level = SessionSaveGame->LvDepth;

    return 100 + (Level * 1000);
}

int32 UNecSaveGameSubsystem::GetRequiredSubmitValue() const
{
    if (!SessionSaveGame) return 500;

    int32 Level = SessionSaveGame->LvDepth;

    return GetLevelMaxSpawnCost() * 1.5f;
}

