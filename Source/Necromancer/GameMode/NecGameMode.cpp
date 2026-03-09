// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/NecGameMode.h"

#include "Game/NecGameState.h"
#include "Controller/NecPlayerController.h"

void ANecGameMode::BeginPlay()
{
	Super::BeginPlay();
}

void ANecGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	ANecPlayerController* NewNecPlayerController = Cast<ANecPlayerController>(NewPlayer);
	if (NewNecPlayerController)
	{
		PlayerControllers.Add(NewNecPlayerController);

		ANecGameState* NecGameState = GetGameState<ANecGameState>();
		if (NecGameState)
		{
			NecGameState->PlayerControllerCount = PlayerControllers.Num();
			NecGameState->OnRep_PlayerControllerCount();
		}
	}
}

void ANecGameMode::StartGame()
{
	ANecGameState* NecGameState = GetGameState<ANecGameState>();
	if (NecGameState)
	{
		NecGameState->SessionState = ESessionState::Playing;
		NecGameState->OnRep_SessionState();
	}
}

void ANecGameMode::OnPlayerDeath(ANecPlayerController* DeadPlayerController)
{
	PlayerControllers.RemoveSingle(DeadPlayerController);
}


