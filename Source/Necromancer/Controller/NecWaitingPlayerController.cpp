
#include "Controller/NecWaitingPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

#include "GameMode/NecWaitingGameMode.h"
#include "UI/ReadyWidget.h"
#include "Kismet/GameplayStatics.h"

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

				//FInputModeUIOnly Mode;
				FInputModeGameAndUI Mode;
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

				//FInputModeUIOnly Mode;
				FInputModeGameAndUI Mode;
				Mode.SetWidgetToFocus(ReadyWidgetInstance->GetCachedWidget());
				SetInputMode(Mode);

				bShowMouseCursor = true;
			}
		}
	}

	if (WaitingBGM)
	{
		UGameplayStatics::SpawnSound2D(this, WaitingBGM);
	}
}

void ANecWaitingPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (InputMappingContext)
			{
				Subsystem->AddMappingContext(InputMappingContext, 0);
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
