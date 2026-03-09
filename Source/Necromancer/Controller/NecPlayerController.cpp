#include "Controller/NecPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "UI/InGameHUDWidget.h"
#include "UI/ReadyWidget.h"

#include "Kismet/GameplayStatics.h"
#include "GameMode/NecGameMode.h"

#include "Game/NecGameState.h"
#include "Game/NecPlayerState.h"

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

void ANecPlayerController::OnPlayerDeath()
{
	if (HasAuthority())
	{
		UnPossess();

		ANecGameMode* NecGM = Cast<ANecGameMode>(GetWorld()->GetAuthGameMode());
		ANecPlayerController* NewViewTarget = nullptr;
		if (NecGM)
		{
			NecGM->OnPlayerDeath(this);
		}
		Client_HandleDeath();
	}
}

void ANecPlayerController::Client_HandleDeath_Implementation()
{
	if (!IsLocalController()) return;

	AGameStateBase* GS = GetWorld()->GetGameState();
	if (GS)
	{
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, [this]()
			{
				AActor* NextTarget = GetNextLivePlayer(nullptr);
				if (NextTarget)
				{
					if (PlayerCameraManager)
					{
						PlayerCameraManager->bDefaultConstrainAspectRatio = false;
					}

					this->SetViewTargetWithBlend(NextTarget, 0.5f);
				}
			}, 0.2f, false);
	}
}

AActor* ANecPlayerController::GetNextLivePlayer(AActor* CurrentViewTarget)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("GetNextLivePlayer 0"));
	ANecGameState* GS = Cast<ANecGameState>(GetWorld()->GetGameState());
	if (!GS) return nullptr;

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("GetNextLivePlayer 1"));
	const TArray<APlayerState*>& PlayerArray = GS->PlayerArray;
	if (PlayerArray.Num() == 0) return nullptr;

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("GetNextLivePlayer 2"));

	// 현재 보고 있는 타겟의 인덱스 찾기
	int32 CurrentIndex = -1;
	if (CurrentViewTarget)
	{
		for (int32 i = 0; i < PlayerArray.Num(); ++i)
		{
			if (PlayerArray[i]->GetPawn() == CurrentViewTarget)
			{
				CurrentIndex = i;
				break;
			}
		}
	}

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("GetNextLivePlayer 3"));

	for (int32 i = 1; i <= PlayerArray.Num(); ++i)
	{
		int32 NextIndex = (CurrentIndex + i) % PlayerArray.Num();
		ANecPlayerState* TargetPS = Cast<ANecPlayerState>(PlayerArray[NextIndex]);

		if (TargetPS && TargetPS != GetPlayerState<ANecPlayerState>())
		{
			if (AActor* TargetPawn = TargetPS->GetPawn())
			{
				return TargetPawn;
			}
		}
	}

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("GetNextLivePlayer 4"));

	return nullptr;
}




void ANecPlayerController::Client_NotifyMonsterKill_Implementation()
{
	UNecSaveGameSubsystem* NecSaveGameSubsystem = GetGameInstance()->GetSubsystem<UNecSaveGameSubsystem>();
	if (NecSaveGameSubsystem)
	{
		NecSaveGameSubsystem->IncreaseProfileKillCount();
	}
}