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

void ANecGameMode::OnPlayerDeath(ANecPlayerController* DeadPC)
{
    PlayerControllers.RemoveSingle(DeadPC);

    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("OnPlayerDeath GAMEMODE %d"), PlayerControllers.Num()));
    Server_ReqeustSpectatingTarget(DeadPC, nullptr, true);
}


void ANecGameMode::Server_ReqeustSpectatingTarget_Implementation(ANecPlayerController* RequestPC, AActor* CurSpectatingTarget, bool isPositiveDirection)
{
    if (!(PlayerControllers.Num() > 0 && PlayerControllers[0])) return;

    if (CurSpectatingTarget == nullptr)
    {
        RequestPC->Client_HandleDeath(PlayerControllers[0]->GetPawn());
    }
    else
    {
        int32 CurIdx = -1;
        for (int32 i = 0; i < PlayerControllers.Num(); ++i)
        {
            if (PlayerControllers[i] && PlayerControllers[i]->GetPawn() == CurSpectatingTarget)
            {
                CurIdx = i;
                break;
            }
        }

        if (CurIdx != -1)
        {
            int32 NextIdx;
            int32 TotalCount = PlayerControllers.Num();

            if (isPositiveDirection)
            {
                NextIdx = (CurIdx + 1) % TotalCount;
            }
            else
            {
                NextIdx = (CurIdx - 1 + TotalCount) % TotalCount;
            }

            if (PlayerControllers[NextIdx])
            {
                RequestPC->Client_HandleDeath(PlayerControllers[NextIdx]->GetPawn());
            }
        }
        else
        {
            RequestPC->Client_HandleDeath(PlayerControllers[0]->GetPawn());
        }
    }

}


