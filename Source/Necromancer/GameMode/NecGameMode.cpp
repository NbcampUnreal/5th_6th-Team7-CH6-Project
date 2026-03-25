// Fill out your copyright notice in the Description page of Project Settings.

#include "GameMode/NecGameMode.h"

#include "SaveGame/NecSaveGameSubsystem.h"
#include "Game/NecGameState.h"

#include "Controller/NecPlayerController.h"

void ANecGameMode::BeginPlay()
{
	Super::BeginPlay();

    // if GameInstance-> GameSlotIdx != -1 then 
    // NecSaveGameSubsystem->InitSessionSaveGame(SlotIdx); 를 호출하여 게임을 SaveGame을 초기화한다
    // else
    // StartGame에서 호출하는 GetLvDepth()가 저장데이터가 없는경우 빈값을 반환하니 SaveGame초기화는 걱정 안해도됨
    // 여기서 InitSessionSaveGame()를 해버리면 매 레벨마다 SaveGame을 새로 초기화해서 이전 레벨간이동시 저장된 데이터가 삭제됨
    StartGame();
}

/// <summary>
/// PIE 테스트 코드(바로 인게임: POST LOGIN)
/// </summary>
/// <param name="NewPlayer">InPlayerController</param>
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

/// <summary>
/// 패키징 후 실전 함수(대기실-> 인게임 : 심리스트레블)
/// </summary>
/// <param name="C">InPlayerController</param>
void ANecGameMode::HandleSeamlessTravelPlayer(AController*& C)
{
    Super::HandleSeamlessTravelPlayer(C);

    ANecPlayerController* NewNecPlayerController = Cast<ANecPlayerController>(C);
    if (NewNecPlayerController)
    {
        PlayerControllers.Add(NewNecPlayerController);

        NewNecPlayerController->Client_CreateIngameHUDWidget();
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


    if (GetWorld()->bIsTearingDown) {
        return;
    }

    // 관전자가 나갓을때, 게임이 종료되는 버그가..있는거같은데 확인필요
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
    InitGameState();
}

void ANecGameMode::InitGameState()
{
    UNecSaveGameSubsystem* NecSaveGameSubsystem = GetGameInstance()->GetSubsystem<UNecSaveGameSubsystem>();
    ANecGameState* NecGameState = GetGameState<ANecGameState>();

    if (NecGameState && NecSaveGameSubsystem)
    {
        NecGameState->LvDepth = NecSaveGameSubsystem->GetLvDepth();
        NecGameState->SubmittedItemValue = NecSaveGameSubsystem->GetSubmittedItemValue();
        NecGameState->KillCount = NecSaveGameSubsystem->GetKillCount();
    }
}

void ANecGameMode::EndGame()
{
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        APlayerController* PC = It->Get();
        if (PC)
        {
            ANecPlayerController* NecPC = Cast<ANecPlayerController>(PC);
            if (NecPC)
            {
                NecPC->Client_CreateEndGameWidget();
            }
        }
    }
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
    if (RevivedPlayerController)
    {
        PlayerControllers.AddUnique(RevivedPlayerController);
    }
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




void ANecGameMode::SaveGameStateToSaveGame()
{
    UNecSaveGameSubsystem* NecSaveGameSubsystem = GetGameInstance()->GetSubsystem<UNecSaveGameSubsystem>();
    if (NecSaveGameSubsystem)
    {
        NecSaveGameSubsystem->SaveSessionSaveGame();
    }
}

void ANecGameMode::IncreaseLvDepth()
{
    ANecGameState* NecGS = GetGameState<ANecGameState>();
    if (NecGS)
    {
        NecGS->LvDepth++;
    }
}

int32 ANecGameMode::GetLvDepth() const
{
    ANecGameState* NecGS = GetGameState<ANecGameState>();
    if (NecGS)
    {
        return NecGS->LvDepth;
    }
    return -1;
}

void ANecGameMode::AddSubmiitedItemValue(int32 Value)
{
    ANecGameState* NecGS = GetGameState<ANecGameState>();
    if (NecGS)
    {
        NecGS->SubmittedItemValue += Value;
    }
}

int32 ANecGameMode::GetSubmiitedItemValue() const
{
    ANecGameState* NecGS = GetGameState<ANecGameState>();
    if (NecGS)
    {
        return NecGS->SubmittedItemValue;
    }
    return -1;
}

void ANecGameMode::IncreaseKillCount()
{
    ANecGameState* NecGS = GetGameState<ANecGameState>();
    if (NecGS)
    {
        NecGS->KillCount++;
    }
}

int32 ANecGameMode::GetKillCount() const
{
    ANecGameState* NecGS = GetGameState<ANecGameState>();
    if (NecGS)
    {
        return NecGS->KillCount;
    }
    return -1;
}
