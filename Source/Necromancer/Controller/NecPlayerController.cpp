#include "Controller/NecPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "UI/InGameHUDWidget.h"
#include "UI/ReadyWidget.h"

#include "Kismet/GameplayStatics.h"
#include "GameMode/NecGameMode.h"

void ANecPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!IsLocalController())
	{
		return;
	}

	if (GetWorld()->GetNetMode() == NM_ListenServer)
	{
		CreateReadyWidgetForHost();
	}
	else
	{
		CreateInGameHUD();
	}

}

void ANecPlayerController::CreateReadyWidgetForHost()
{
	if (IsValid(ReadyWidgetClass))
	{
		ReadyWidgetInstance = CreateWidget<UReadyWidget>(this, ReadyWidgetClass);
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

void ANecPlayerController::CreateInGameHUD()
{
	FInputModeGameOnly GameOnly;
	SetInputMode(GameOnly);
	bShowMouseCursor = false;

	if (IsValid(InGameHUDWidgetClass))
	{
		InGameHUDWidgetInstance = CreateWidget<UInGameHUDWidget>(this, InGameHUDWidgetClass);
		if (IsValid(InGameHUDWidgetInstance))
		{
			InGameHUDWidgetInstance->AddToViewport(1);
		}
	}
}

void ANecPlayerController::SetupInputComponent()
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

void ANecPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (IsValid(InGameHUDWidgetInstance))
	{
		InGameHUDWidgetInstance->InitHUD();
	}
}

/// <summary>
/// Only Host Can call this Function(Using ReadyWidet)
/// </summary>
void ANecPlayerController::OnStartGame()
{
	if (ReadyWidgetInstance)
	{
		ReadyWidgetInstance->RemoveFromParent();
	}

	ANecGameMode* NecGameMode = Cast<ANecGameMode>(UGameplayStatics::GetGameMode(this));
	if (NecGameMode)
	{
		NecGameMode->StartGame();
		CreateInGameHUD();
	}
}
