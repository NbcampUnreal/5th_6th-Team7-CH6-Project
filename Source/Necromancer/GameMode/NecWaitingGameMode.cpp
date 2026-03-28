// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/NecWaitingGameMode.h"

#include "Game/NecGameState.h"

ANecWaitingGameMode::ANecWaitingGameMode()
{
	bUseSeamlessTravel = true;
}

void ANecWaitingGameMode::BeginPlay()
{
	Super::BeginPlay();
}

void ANecWaitingGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	PlayerControllers.Add(NewPlayer);

	ANecGameState* NecGameState = GetGameState<ANecGameState>();
	if (NecGameState)
	{
		NecGameState->PlayerControllerCount = PlayerControllers.Num();
		NecGameState->OnRep_PlayerControllerCount();
	}
}

void ANecWaitingGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	PlayerControllers.RemoveSingle(Cast<APlayerController>(Exiting));
	ANecGameState* NecGameState = GetGameState<ANecGameState>();
	if (NecGameState)
	{
		NecGameState->PlayerControllerCount = PlayerControllers.Num();
		NecGameState->OnRep_PlayerControllerCount();
	}
}

void ANecWaitingGameMode::StartGame()
{
	GetWorld()->ServerTravel("/Game/Necromancer/Maps/InDungeonLevel?listen",true );
}
