#include "Controller/NecPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "Net/UnrealNetwork.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/UserWidget.h"
#include "UI/ReadyWidget.h"
#include "UI/InGameHUDWidget.h"
#include "UI/EndGame.h"

#include "Kismet/GameplayStatics.h"
#include "GameMode/NecGameMode.h"

#include "Game/NecGameState.h"
#include "Game/NecPlayerState.h"
#include "Character/NecPlayerCharacter.h"
#include "Component/StatComponent.h"
#include "Component/SoulComponent.h"

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

	CreateInGameHUD();
}

void ANecPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	MyBody = Cast<ANecPlayerCharacter>(GetCharacter());
	if (IsValid(MyBody))
	{
		USoulComponent* MySoulComp = MyBody->GetSoulComponent();
		if (IsValid(MySoulComp))
		{
			if (MySoulComp->OnReviveRequested.IsAlreadyBound(this, &ANecPlayerController::HandleRevive))
			{
				MySoulComp->OnReviveRequested.RemoveDynamic(this, &ANecPlayerController::HandleRevive);
			}

			MySoulComp->OnReviveRequested.AddDynamic(this, &ANecPlayerController::HandleRevive);
		}
	}
}

void ANecPlayerController::Client_CreateEndGameWidget_Implementation()
{
	TArray<UUserWidget*> FoundWidgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), FoundWidgets, UUserWidget::StaticClass(), false);

	for (UUserWidget* Widget : FoundWidgets)
	{
		if (Widget && Widget->IsInViewport())
		{
			Widget->RemoveFromParent();
		}
	}

	if (EndGameWidgetClass)
	{
		EndGameWidgetInstance = CreateWidget<UEndGame>(this, EndGameWidgetClass);
		if (EndGameWidgetInstance)
		{
			EndGameWidgetInstance->AddToViewport();
			EndGameWidgetInstance->InitGameScore();

			FInputModeUIOnly InputMode;
			SetInputMode(InputMode);
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

void ANecPlayerController::CreateSpectatorHUD()
{
	if (IsValid(SpectatorHUDWidgetInstance) == false)
	{
		if (IsValid(SpectatorHUDWidgetClass))
		{
			SpectatorHUDWidgetInstance = CreateWidget<UInGameHUDWidget>(this, SpectatorHUDWidgetClass);
		}
	}

	if (IsValid(SpectatorHUDWidgetInstance) == true)
	{
		if (IsValid(InGameHUDWidgetInstance) && InGameHUDWidgetInstance->IsInViewport())
		{
			InGameHUDWidgetInstance->RemoveFromParent();
		}

		if (!SpectatorHUDWidgetInstance->IsInViewport() && 
			(EndGameWidgetInstance != nullptr && !EndGameWidgetInstance->IsInViewport()))
		{
			SpectatorHUDWidgetInstance->AddToViewport(1);
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

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (SpectatingTargetUpAction)
		{
			EnhancedInputComponent->BindAction(SpectatingTargetUpAction, ETriggerEvent::Triggered, this, &ANecPlayerController::SpectatingTargetUp);
		}

		if (SpectatingTargetDownAction)
		{
			EnhancedInputComponent->BindAction(SpectatingTargetDownAction, ETriggerEvent::Triggered, this, &ANecPlayerController::SpectatingTargetDown);
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

void ANecPlayerController::SpectatingTargetUp()
{
	if (!IsLocalController() || !bIsSpectating) return;

	Server_RequestSpectatingTarget(SpectatingTarget, true);
}

void ANecPlayerController::SpectatingTargetDown()
{
	if (!IsLocalController() || !bIsSpectating) return;

	Server_RequestSpectatingTarget(SpectatingTarget, false);
}



void ANecPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, bIsSpectating);
}

void ANecPlayerController::OnPlayerDeath()
{
	bIsSpectating = true;
	Server_NotifyDeath();

	CreateSpectatorHUD();
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

void ANecPlayerController::Client_HandleCameraTarget_Implementation(AActor* TargetToSpectate)
{
	if (!IsLocalController()) return;

	AGameStateBase* GS = GetWorld()->GetGameState();
	if (GS)
	{
		GetWorldTimerManager().ClearTimer(SpectateRotationTimerHandle);

		GetWorldTimerManager().SetTimer(SpectateRotationTimerHandle, [this, TargetToSpectate]()
			{
				SetSpectateTargetInternal(TargetToSpectate);
			}, 0.2f, false);
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
			GetWorldTimerManager().ClearTimer(SpectateRotationTimerHandle);

			Server_RequestSpectatingTarget(SpectatingTarget, true);
		}
	}
	else
	{
		GetWorldTimerManager().ClearTimer(SpectateRotationTimerHandle);
	}
}

void ANecPlayerController::Server_RequestSpectatingTarget_Implementation(AActor* InSpectatingTarget, bool bIsUp)
{
	if (ANecGameMode* NecGM = Cast<ANecGameMode>(GetWorld()->GetAuthGameMode()))
	{
		NecGM->Server_ReqeustSpectatingTarget(this, InSpectatingTarget, bIsUp);
	}
}

void ANecPlayerController::HandleRevive()
{
	if (HasAuthority() && MyBody)
	{
		// possess
		Possess(MyBody);
		bIsSpectating = false;

		if (ANecGameMode* NecGM = Cast<ANecGameMode>(GetWorld()->GetAuthGameMode()))
		{
			NecGM->OnPlayerRevive(this);
		}

		// and controller camera view
		Client_HandleCameraTarget(MyBody);
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