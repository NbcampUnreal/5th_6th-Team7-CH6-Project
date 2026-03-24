// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/EndGame.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

#include "Game/NecGameState.h"
#include "GameInstance/NecAFGameInstance.h"

void UEndGame::NativeConstruct()
{
	Super::NativeConstruct();

	RestartGameButton->OnClicked.AddDynamic(this, &UEndGame::OnRestartGameButtonClicked);
	GoLobbyButton->OnClicked.AddDynamic(this, &UEndGame::OnGoLobbyButtonClicked);
}

void UEndGame::OnRestartGameButtonClicked()
{
    UNecAFGameInstance* NecAFGameInstance = Cast<UNecAFGameInstance>(GetGameInstance());
    if (NecAFGameInstance)
    {
        NecAFGameInstance->CreateSession();
    }
}

void UEndGame::OnGoLobbyButtonClicked()
{
    APlayerController* PC = GetOwningPlayer();
    if (!PC) return;

    if (PC->HasAuthority())
    {
        PC->ClientReturnToMainMenuWithTextReason(FText::FromString(TEXT("Host has ended the session.")));
    }
    else
    {
        PC->ClientReturnToMainMenuWithTextReason(FText::FromString(TEXT("Leaving the game.")));
    }
}

void UEndGame::InitGameScore()
{
    ANecGameState* NecGS = GetWorld()->GetGameState<ANecGameState>();
	if (NecGS)
	{
        if (ClearLvDepthText)
        {
            FString CombinedString = FString::Printf(TEXT("총 내려간 지하 깊이: %d"), (NecGS->LvDepth+1) * -100);
            ClearLvDepthText->SetText(FText::FromString(CombinedString));
        }

        if (TotalSubmittedItemValueText)
        {
            FString CombinedString = FString::Printf(TEXT("총 제출한 아이템 가치: %d"), NecGS->SubmittedItemValue);
            TotalSubmittedItemValueText->SetText(FText::FromString(CombinedString));
        }

        if (TotalKillCountText)
        {
            FString CombinedString = FString::Printf(TEXT("총 처치한 몬스터 수: %d"), NecGS->KillCount);
            TotalKillCountText->SetText(FText::FromString(CombinedString));
        }
	}
}
