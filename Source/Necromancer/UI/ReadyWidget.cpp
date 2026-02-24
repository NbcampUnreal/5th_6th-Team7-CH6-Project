// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ReadyWidget.h"

#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

#include "GameInstance/NecAFGameInstance.h"
#include "GameMode/NecWaitingGameMode.h"
#include "Game/NecGameState.h"
#include "Controller/NecWaitingPlayerController.h"

void UReadyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	StartGameButton->OnClicked.AddDynamic(this, &UReadyWidget::OnStartGameButtonClicked);
	InviteFriendButton->OnClicked.AddDynamic(this, &UReadyWidget::OnInviteFriendButtonClicked);

	if (ANecGameState* NecGameState = GetWorld()->GetGameState<ANecGameState>())
	{
		NecGameState->OnPlayerControllerCountChanged.AddDynamic(this, &UReadyWidget::UpdatePlayerControllerCount);
		UpdatePlayerControllerCount(NecGameState->PlayerControllerCount);
	}
}

void UReadyWidget::OnStartGameButtonClicked()
{
	ANecWaitingGameMode* NecWaitingGM = Cast<ANecWaitingGameMode>(GetWorld()->GetAuthGameMode());
	if (NecWaitingGM)
	{
		NecWaitingGM->StartGame();
	}

	//ANecWaitingPlayerController* NecWaitingPC = Cast<ANecWaitingPlayerController>(GetOwningPlayer());
	//if (NecWaitingPC)
	//{
	//	NecWaitingPC->StartGame();
	//}
}

void UReadyWidget::OnInviteFriendButtonClicked()
{
	UNecAFGameInstance* NecAFGameInstance = Cast<UNecAFGameInstance>(GetGameInstance());
	if (NecAFGameInstance)
	{
		NecAFGameInstance->InviteFriend();
	}
}

void UReadyWidget::UpdatePlayerControllerCount(int32 PlayerCount)
{
	if (PlayerCountText)
	{
		FString PlayerCountString = FString::Printf(TEXT("Player: %d"), PlayerCount);
		PlayerCountText->SetText(FText::FromString(PlayerCountString));
	}
}
