// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/LobbyWidget.h"

#include "Components/Button.h"

#include "Controller/NecLobbyPlayerController.h"

void ULobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CreateSessionButton.Get()->OnClicked.AddDynamic(this, &ThisClass::OnCreateSessionButtonClicked);
	FindSessionButton.Get()->OnClicked.AddDynamic(this, &ThisClass::OnFindSessionButtonClicked);
	InviteFirendButton.Get()->OnClicked.AddDynamic(this, &ThisClass::OnInviteFriendButtonClicked);
}

void ULobbyWidget::OnCreateSessionButtonClicked()
{
	ANecLobbyPlayerController* OwningPlayerController = GetOwningPlayer<ANecLobbyPlayerController>();
	if (IsValid(OwningPlayerController))
	{
		OwningPlayerController->OnClickCreateSession();
	}
}

void ULobbyWidget::OnFindSessionButtonClicked()
{
	ANecLobbyPlayerController* OwningPlayerController = GetOwningPlayer<ANecLobbyPlayerController>();
	if (IsValid(OwningPlayerController))
	{
		OwningPlayerController->OnClickFindSession();
	}
}

void ULobbyWidget::OnInviteFriendButtonClicked()
{
	ANecLobbyPlayerController* OwningPlayerController = GetOwningPlayer<ANecLobbyPlayerController>();
	if (IsValid(OwningPlayerController))
	{
		OwningPlayerController->OnClickInviteFriend();
	}
}
