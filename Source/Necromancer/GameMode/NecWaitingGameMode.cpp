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

void ANecWaitingGameMode::StartGame()
{
	//GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Blue, FString::Printf(TEXT("StartGame")));
	GetWorld()->ServerTravel("/Game/Necromancer/Maps/InDungeonLevel?listen",true );
}
