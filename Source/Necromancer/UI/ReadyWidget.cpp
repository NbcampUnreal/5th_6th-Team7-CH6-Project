// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ReadyWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

#include "GameInstance/NecAFGameInstance.h"
#include "Game/NecGameState.h"
#include "Controller/NecPlayerController.h"

void UReadyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	StartGameButton->OnClicked.AddDynamic(this, &UReadyWidget::OnStartGameButtonClicked);
	InviteFriendButton->OnClicked.AddDynamic(this, &UReadyWidget::OnInviteFriendButtonClicked);

	if (ANecGameState* NecGameState = GetWorld()->GetGameState<ANecGameState>())
	{
		NecGameState->OnPlayerControllerCountChanged.AddDynamic(this, &ThisClass::UpdatePlayerControllerCount);
		UpdatePlayerControllerCount(NecGameState->PlayerControllerCount);
	}
}

void UReadyWidget::OnStartGameButtonClicked()
{
	ANecPlayerController* PC = Cast<ANecPlayerController>(GetOwningPlayer());
	if (PC)
	{
		PC->OnStartGame();
	}
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
