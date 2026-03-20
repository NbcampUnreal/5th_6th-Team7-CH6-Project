// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/EndGame.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

#include "SaveGame/NecSaveGameSubsystem.h"

void UEndGame::NativeConstruct()
{
	Super::NativeConstruct();

	RestartGameButton->OnClicked.AddDynamic(this, &UEndGame::OnRestartGameButtonClicked);
	GoLobbyButton->OnClicked.AddDynamic(this, &UEndGame::OnGoLobbyButtonClicked);
}

void UEndGame::OnRestartGameButtonClicked()
{
	// 연결 다끊고 시작
}

void UEndGame::OnGoLobbyButtonClicked()
{
	// 연결 끊고 로비로
}

void UEndGame::InitGameScore()
{
	UNecSaveGameSubsystem* NecSaveGameSubsystem = GetGameInstance()->GetSubsystem<UNecSaveGameSubsystem>();
	if (NecSaveGameSubsystem)
	{
        if (ClearLvDepthText)
        {
            int32 LvDepth = NecSaveGameSubsystem->GetLvDepth();

            FString CombinedString = FString::Printf(TEXT("총 클리어한 스테이지 수: %d"), LvDepth);
            ClearLvDepthText->SetText(FText::FromString(CombinedString));
        }

        if (TotalKillCountText)
        {
            int32 KillCount = NecSaveGameSubsystem->GetKillCount();

            FString CombinedString = FString::Printf(TEXT("총 처치한 몬스터 수: %d"), KillCount);
            TotalKillCountText->SetText(FText::FromString(CombinedString));
        }
	}
}
