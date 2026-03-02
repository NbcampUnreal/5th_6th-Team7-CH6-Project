// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveGame/NecSaveGameSubsystem.h"

#include "Kismet/GameplayStatics.h"

#include "SaveGame/NecProfileSaveGame.h"

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

void UNecSaveGameSubsystem::IncreaseKillCount()
{
    ProfileSaveGame->TotalKillCount++;
    UE_LOG(LogTemp, Warning, TEXT("Kill Count Updated! Total: %d"), ProfileSaveGame ? ProfileSaveGame->TotalKillCount : 0);
}

void UNecSaveGameSubsystem::SaveProfileSaveGame()
{
    UGameplayStatics::SaveGameToSlot(ProfileSaveGame, ProfileSaveGameSlotName, 0);
}
