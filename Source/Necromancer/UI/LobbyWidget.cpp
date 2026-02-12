// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/LobbyWidget.h"

#include "Components/Button.h"

#include "Controller/NecLobbyPlayerController.h"
#include "GameInstance/NecAFGameInstance.h"

void ULobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CreateSessionButton.Get()->OnClicked.AddDynamic(this, &ThisClass::OnCreateSessionButtonClicked);
	InviteFirendButton.Get()->OnClicked.AddDynamic(this, &ThisClass::OnInviteFriendButtonClicked);
}

void ULobbyWidget::OnCreateSessionButtonClicked()
{
	UNecAFGameInstance* NecAFGameInstance = Cast<UNecAFGameInstance>(GetGameInstance());
	if (NecAFGameInstance)
	{
		NecAFGameInstance->CreateSession();
	}
}

void ULobbyWidget::OnInviteFriendButtonClicked()
{
	UNecAFGameInstance* NecAFGameInstance = Cast<UNecAFGameInstance>(GetGameInstance());
	if (NecAFGameInstance)
	{
		NecAFGameInstance->InviteFriend();
	}
}
