#include "Controller/NecPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "UI/InGameHUDWidget.h"
#include "UI/ReadyWidget.h"

#include "Kismet/GameplayStatics.h"
#include "GameMode/NecGameMode.h"

#include "Game/NecGameState.h"
#include "Game/NecPlayerState.h"
#include "Character/NecPlayerCharacter.h"
#include "Component/StatComponent.h"

#include "GameInstance/NecAFGameInstance.h"
#include "SaveGame/NecSaveGameSubsystem.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

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
	bIsSpectating = true;
	Server_NotifyDeath();
}

void ANecPlayerController::Server_NotifyDeath_Implementation()
{
	bIsSpectating = true;
	UnPossess();

	if (ANecGameMode* NecGM = Cast<ANecGameMode>(GetWorld()->GetAuthGameMode()))
	{
		NecGM->OnPlayerDeath(this);
	}
}

// 이거 이름 바꾸는게 나을듯? 클라이언트_핸들 SetSpectating? 
void ANecPlayerController::Client_HandleDeath_Implementation(AActor* TargetToSpectate)
{
	if (!IsLocalController()) return;

	AGameStateBase* GS = GetWorld()->GetGameState();
	if (GS)
	{
		GetWorldTimerManager().SetTimer(SpectateRotationTimerHandle, [this, TargetToSpectate]()
			{
				SetSpectateTargetInternal(TargetToSpectate);
			}, 0.2f, false);
	}
}

void ANecPlayerController::Server_RequestSpectatingTarget_Implementation()
{
	if (ANecGameMode* NecGM = Cast<ANecGameMode>(GetWorld()->GetAuthGameMode()))
	{
		NecGM->Server_ReqeustSpectatingTarget(this, SpectatingTarget, true);
	}
}

void ANecPlayerController::SetSpectateTargetInternal(AActor* TargetToSpectate)
{
	ANecPlayerCharacter* TargetCharacter = Cast<ANecPlayerCharacter>(TargetToSpectate);
	if (TargetCharacter)
	{
		SpectatingTarget = TargetToSpectate;
		CurSpectatingTargetState = TargetCharacter->GetPlayerState<ANecPlayerState>();

		bAutoManageActiveCameraTarget = true;
		this->SetViewTargetWithBlend(TargetToSpectate, 0.5f);

		GetWorldTimerManager().SetTimer(
			SpectateRotationTimerHandle,
			this,
			&ANecPlayerController::UpdateSpectateRotation,
			0.02f,
			true
		);
	}
}

void ANecPlayerController::UpdateSpectateRotation()
{
	if (IsLocalController() && SpectatingTarget && CurSpectatingTargetState)
	{
		ANecPlayerCharacter* TargetCharacter = Cast<ANecPlayerCharacter>(SpectatingTarget);
		if (TargetCharacter == nullptr) return;

		if (CurSpectatingTargetState->GetStatComponent()->GetIsDead() == false)
		{
			FRotator TargetRot = TargetCharacter->RemoteViewRot;
			if (HasAuthority())
			{
				TargetRot = TargetCharacter->GetControlRotation();
			}
			else
			{
				TargetRot = TargetCharacter->RemoteViewRot;
			}

			USpringArmComponent* SpringArm = TargetCharacter->FindComponentByClass<USpringArmComponent>();
			if (SpringArm)
			{
				SpringArm->bUsePawnControlRotation = false;
				SpringArm->SetWorldRotation(TargetRot);

				TargetCharacter->bUseControllerRotationYaw = false;
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Current TargetPawnState->GetStatComponent()-> is dead "));

			GetWorldTimerManager().ClearTimer(SpectateRotationTimerHandle);

			Server_RequestSpectatingTarget();
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("not IsLocalController() && SpectatingTarget && CurSpectatingTargetState"));
		GetWorldTimerManager().ClearTimer(SpectateRotationTimerHandle);
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