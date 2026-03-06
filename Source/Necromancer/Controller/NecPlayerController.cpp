#include "Controller/NecPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "UI/InGameHUDWidget.h"
#include "UI/ReadyWidget.h"

#include "Kismet/GameplayStatics.h"
#include "GameMode/NecGameMode.h"

#include "GameInstance/NecAFGameInstance.h"
#include "SaveGame/NecSaveGameSubsystem.h"

ANecPlayerController::ANecPlayerController()
{
	static ConstructorHelpers::FClassFinder<UInGameHUDWidget> HUDWidgetFinder(
		TEXT("/Game/Necromancer/Blueprints/UI/WBP_InGameHUD")
	);
	
	if (HUDWidgetFinder.Succeeded())
	{
		InGameHUDWidgetClass = HUDWidgetFinder.Class;
	}
}

void ANecPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!IsLocalController())
	{
		return;
	}

	FString MapName = GetWorld()->GetMapName();
	if (MapName.Contains("Lobby"))
	{
		if (GetWorld()->GetNetMode() == NM_ListenServer)
		{
			CreateReadyWidgetForHost();
		}
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

	if (IsValid(InGameHUDWidgetInstance))
	{
		if (!InGameHUDWidgetInstance->IsInViewport())
		{
			InGameHUDWidgetInstance->AddToViewport(1);
		}

		InGameHUDWidgetInstance->InitHUD();
		return;
	}

	if (IsValid(InGameHUDWidgetClass))
	{
		InGameHUDWidgetInstance = CreateWidget<UInGameHUDWidget>(this, InGameHUDWidgetClass);
		if (IsValid(InGameHUDWidgetInstance))
		{
			InGameHUDWidgetInstance->AddToViewport(1);
			InGameHUDWidgetInstance->InitHUD();
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

void ANecPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (IsLocalController())
	{
		FString MapName = GetWorld()->GetMapName();
		if (!MapName.Contains("Lobby"))
		{
			CreateInGameHUD();
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

void ANecPlayerController::OnRep_Pawn()
{
	Super::OnRep_Pawn();

	if (IsLocalController())
	{
		FString MapName = GetWorld()->GetMapName();
		if (!MapName.Contains("Lobby"))
		{
			CreateInGameHUD();
		}
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

void ANecPlayerController::Client_NotifyMonsterKill_Implementation()
{
	UNecSaveGameSubsystem* NecSaveGameSubsystem = GetGameInstance()->GetSubsystem<UNecSaveGameSubsystem>();
	if (NecSaveGameSubsystem)
	{
		NecSaveGameSubsystem->IncreaseProfileKillCount();
	}
}
