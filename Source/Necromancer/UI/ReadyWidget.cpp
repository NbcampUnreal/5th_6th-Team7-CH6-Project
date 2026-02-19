// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ReadyWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

#include "GameInstance/NecAFGameInstance.h"
#include "Controller/NecPlayerController.h"

void UReadyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	StartGameButton->OnClicked.AddDynamic(this, &UReadyWidget::OnStartGameButtonClicked);
	InviteFriendButton->OnClicked.AddDynamic(this, &UReadyWidget::OnInviteFriendButtonClicked);
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
