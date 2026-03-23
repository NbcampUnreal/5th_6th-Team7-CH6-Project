// Fill out your copyright notice in the Description page of Project Settings.

#include "NecSaveGameSubsystem.h"
#include "SaveGame/NecSaveGameSubsystem.h"

#include "Kismet/GameplayStatics.h"

#include "SaveGame/NecProfileSaveGame.h"
#include "SaveGame/NecSessionSaveGame.h"

#include "Game/NecGameState.h"

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

/// <summary>
/// 슬롯불러오기를 할경우 GameInstance SlotIdx를 저장한후, 
/// </summary>
/// <param name="SlotIdx"></param>
void UNecSaveGameSubsystem::InitSessionSaveGame(int32 SlotIdx)
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

/// <summary>
/// SessionData = GameState SessionData
/// </summary>
/// <param name="SlotIdx">
/// 세션에서 레벨간 이동을 위해 데이터를 저장하는 경우라면 -1을 이용
/// 현재 게임정보를 저장하기 위함이라면, index = in(0,1,2)
/// </param>
void UNecSaveGameSubsystem::SaveSessionSaveGame(int32 SlotIdx)
{
    if (SessionSaveGame == nullptr) 
    {
        InitSessionSaveGame(SlotIdx);
    }

    ANecGameState* NecGS = GetWorld()->GetGameState<ANecGameState>();
    if (NecGS)
    {
        SessionSaveGame->LvDepth = NecGS->LvDepth;
        SessionSaveGame->SubmittedItemValue = NecGS->SubmittedItemValue;
        SessionSaveGame->KillCount = NecGS->KillCount;

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
}

int32 UNecSaveGameSubsystem::GetLvDepth()
{
    return SessionSaveGame ? SessionSaveGame->LvDepth : 0;
}

int32 UNecSaveGameSubsystem::GetKillCount() const
{
    return SessionSaveGame ? SessionSaveGame->KillCount : 0;
}