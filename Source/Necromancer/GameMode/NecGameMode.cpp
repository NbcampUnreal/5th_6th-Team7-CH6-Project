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

void ANecGameMode::Logout(AController* Exiting)
{
    Super::Logout(Exiting);

    ANecPlayerController* ExitingNecPC = Cast<ANecPlayerController>(Exiting);
    if (ExitingNecPC)
    {
        if (PlayerControllers.RemoveSingle(ExitingNecPC) > 0)
        {
            if (PlayerControllers.Num() == 0)
            {
                EndGame();
            }
        }
    }
}

void ANecGameMode::StartGame()
{
	ANecGameState* NecGameState = GetGameState<ANecGameState>();
	if (NecGameState)
	{
		//NecGameState->SessionState = ESessionState::Playing;
		//NecGameState->OnRep_SessionState();

        // 별도 관리할 대상이 없음.. 
	}
}

void ANecGameMode::EndGame()
{
    // UI 출력 및 게임 종료 
}

void ANecGameMode::OnPlayerDeath(ANecPlayerController* DeadPC)
{
    PlayerControllers.RemoveSingle(DeadPC);

    if (PlayerControllers.Num() < 1)
    {
        EndGame();
    }
    else
    {
        Server_ReqeustSpectatingTarget(DeadPC, nullptr, true);
    }
}

void ANecGameMode::OnPlayerRevive(ANecPlayerController* RevivedPlayerController)
{
    PlayerControllers.Add(RevivedPlayerController);
}


void ANecGameMode::Server_ReqeustSpectatingTarget_Implementation(ANecPlayerController* RequestPC, AActor* CurSpectatingTarget, bool bIsUp)
{
    if (!(PlayerControllers.Num() > 0 && PlayerControllers[0])) 
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("There are no Target To Spectating"));
        return;
    }

    if (CurSpectatingTarget == nullptr)
    {
        RequestPC->Client_HandleCameraTarget(PlayerControllers[0]->GetPawn());
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

            if (bIsUp)
            {
                NextIdx = (CurIdx + 1) % TotalCount;
            }
            else
            {
                NextIdx = (CurIdx - 1 + TotalCount) % TotalCount;
            }

            if (PlayerControllers[NextIdx])
            {
                RequestPC->Client_HandleCameraTarget(PlayerControllers[NextIdx]->GetPawn());
            }
        }
        else
        {
            RequestPC->Client_HandleCameraTarget(PlayerControllers[0]->GetPawn());
        }
    }

}


