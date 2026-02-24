
#include "Controller/NecWaitingPlayerController.h"


#include "GameMode/NecWaitingGameMode.h"
#include "UI/ReadyWidget.h"

ANecWaitingPlayerController::ANecWaitingPlayerController()
{
}

void ANecWaitingPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!IsLocalController()) 
	{
		return;
	}

	if (GetWorld()->GetNetMode() == NM_ListenServer)
	{
		if (IsValid(HostReadyWidgetClass))
		{
			ReadyWidgetInstance = CreateWidget<UReadyWidget>(this, HostReadyWidgetClass);
			if (IsValid(ReadyWidgetInstance))
			{
				ReadyWidgetInstance->AddToViewport();

				FInputModeUIOnly Mode;
				Mode.SetWidgetToFocus(ReadyWidgetInstance->GetCachedWidget());
				SetInputMode(Mode);

				bShowMouseCursor = true;
			}
		}
	}
	else
	{
		if (IsValid(ClientReadyWidgetClass))
		{
			ReadyWidgetInstance = CreateWidget<UReadyWidget>(this, ClientReadyWidgetClass);
			if (IsValid(ReadyWidgetInstance))
			{
				ReadyWidgetInstance->AddToViewport();

				FInputModeUIOnly Mode;
				Mode.SetWidgetToFocus(ReadyWidgetInstance->GetCachedWidget());
				SetInputMode(Mode);

				bShowMouseCursor = true;
			}
		}
	}
}

void ANecWaitingPlayerController::StartGame()
{
	if (HasAuthority() == false) return;

	ANecWaitingGameMode* NecWaitingGM = Cast<ANecWaitingGameMode>(GetWorld()->GetAuthGameMode());
	if (NecWaitingGM)
	{
		NecWaitingGM->StartGame();
	}
}
