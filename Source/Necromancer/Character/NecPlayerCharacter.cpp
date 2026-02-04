#include "Character/NecPlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Controller/NecPlayerController.h"
#include "Game/NecPlayerState.h"
#include "EnhancedInputComponent.h"
#include "Necromancer.h"
#include "Component/StatComponent.h"
#include "Component/StaminaComponent.h"
#include "Component/PlayerMovementComponent.h"
#include "Component/CombatComponent.h"
#include "Component/TargetingComponent.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "Engine/StaticMeshActor.h"

ANecPlayerCharacter::ANecPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;	

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);	

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 300.0f;
	SpringArmComponent->bUsePawnControlRotation = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
	CameraComponent->bUsePawnControlRotation = false;

	PlayerMovementComponent = CreateDefaultSubobject<UPlayerMovementComponent>(TEXT("PlayerMovementComponent"));
	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	TargetingComponent = CreateDefaultSubobject<UTargetingComponent>(TEXT("TargetingComponent"));
}

void ANecPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	

}

void ANecPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (TargetingComponent && TargetingComponent->GetCurrentTarget())
	{
		AActor* Target = TargetingComponent->GetCurrentTarget();

		FVector Start = GetActorLocation();
		FVector End = Target->GetActorLocation();

		End.Z -= 20.0f;

		FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(Start, End);

		APlayerController* PC = Cast<APlayerController>(GetController());
		if (PC)
		{
			FRotator CurrentRot = PC->GetControlRotation();
			FRotator TargetRot = FMath::RInterpTo(CurrentRot, LookAtRot, DeltaTime, 10.0f);

			PC->SetControlRotation(FRotator(TargetRot.Pitch, TargetRot.Yaw, 0.0f));
		}
	}
}

void ANecPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComp = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (ANecPlayerController* PlayerController = Cast<ANecPlayerController>(GetController()))
		{
			if (PlayerController->MoveAction)
			{
				EnhancedInputComp->BindAction(
					PlayerController->MoveAction,
					ETriggerEvent::Triggered,
					this,
					&ANecPlayerCharacter::Move
				);
			}

			if (PlayerController->LookAction)
			{
				EnhancedInputComp->BindAction(
					PlayerController->LookAction,
					ETriggerEvent::Triggered,
					this,
					&ANecPlayerCharacter::Look
				);
			}

			if (PlayerController->SprintAction)
			{
				EnhancedInputComp->BindAction(
					PlayerController->SprintAction,
					ETriggerEvent::Triggered,
					this,
					&ANecPlayerCharacter::StartSprint
				);

				EnhancedInputComp->BindAction(
					PlayerController->SprintAction,
					ETriggerEvent::Completed,
					this,
					&ANecPlayerCharacter::StopSprint
				);
			}

			if (PlayerController->AttackAction)
			{
				EnhancedInputComp->BindAction(
					PlayerController->AttackAction,
					ETriggerEvent::Started,
					this,
					&ANecPlayerCharacter::Attack
				);
			}

			if (PlayerController->GuardAction)
			{
				EnhancedInputComp->BindAction(
					PlayerController->GuardAction,
					ETriggerEvent::Started,
					this,
					&ANecPlayerCharacter::StartGuard
				);

				EnhancedInputComp->BindAction(
					PlayerController->GuardAction,
					ETriggerEvent::Completed,
					this,
					&ANecPlayerCharacter::StopGuard
				);
			}

			if (PlayerController->LockOnAction)
			{
				EnhancedInputComp->BindAction(
					PlayerController->LockOnAction,
					ETriggerEvent::Started,
					this,
					&ANecPlayerCharacter::LockOn
				);
			}

			if (PlayerController->MenuAction)
			{
				EnhancedInputComp->BindAction(
					PlayerController->MenuAction,
					ETriggerEvent::Started,
					this,
					&ANecPlayerCharacter::ToggleMenu
				);
			}

			if (PlayerController->InteractAction)
			{
				EnhancedInputComp->BindAction(
					PlayerController->InteractAction,
					ETriggerEvent::Started,
					this,
					&ANecPlayerCharacter::Interact
				);
			}
		}
	}	
}

void ANecPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	LinkPlayerStateComponents();
}

void ANecPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	LinkPlayerStateComponents();
}

void ANecPlayerCharacter::Move(const FInputActionValue& Value)
{
	if (IsValid(PlayerMovementComponent))
	{
		PlayerMovementComponent->ProcessMove(Value.Get<FVector2D>());
	}
}

void ANecPlayerCharacter::Look(const FInputActionValue& Value)
{
	if (TargetingComponent && TargetingComponent->GetCurrentTarget())
	{
		return;
	}

	if (IsValid(PlayerMovementComponent))
	{
		PlayerMovementComponent->ProcessLook(Value.Get<FVector2D>());
	}
}

void ANecPlayerCharacter::StartSprint(const FInputActionValue& Value)
{
	if (!IsValid(StaminaComponent) || !IsValid(PlayerMovementComponent))
	{
		return;
	}

	if (StaminaComponent->IsExhausted())
	{
		if (PlayerMovementComponent->GetIsSprinting())
		{
			StopSprint(Value);
		}
		return;
	}

	PlayerMovementComponent->SetSprint(true);
	GetCharacterMovement()->MaxWalkSpeed = PlayerMovementComponent->GetSprintSpeed();

	StaminaComponent->StartStaminaDrain(10.0f);

	Server_SetSprint(true);
}

void ANecPlayerCharacter::StopSprint(const FInputActionValue& Value)
{
	if (IsValid(PlayerMovementComponent))
	{		
		PlayerMovementComponent->SetSprint(false);
		GetCharacterMovement()->MaxWalkSpeed = PlayerMovementComponent->GetNormalSpeed();

		bool bResetExhaustion = StaminaComponent->GetCurrentStamina() > 0.0f;
		StaminaComponent->StopStaminaDrain(bResetExhaustion);

		Server_SetSprint(false);
	}
}

void ANecPlayerCharacter::Attack(const FInputActionValue& Value)
{
	if (IsValid(CombatComponent))
	{
		CombatComponent->Attack();
	}
}

void ANecPlayerCharacter::StartGuard(const FInputActionValue& Value)
{
	if (CombatComponent)
	{		
		CombatComponent->SetGuard(true);
	}
}

void ANecPlayerCharacter::StopGuard(const FInputActionValue& Value)
{
	if (CombatComponent)
	{
		CombatComponent->SetGuard(false);
	}
}

void ANecPlayerCharacter::LockOn(const FInputActionValue& Value)
{
	if (IsValid(CombatComponent))
	{
		TargetingComponent->ToggleLockOn();
	}
}

void ANecPlayerCharacter::ToggleMenu(const FInputActionValue& Value)
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC || !InGameMenuClass)
	{
		return;
	}

	if (!InGameMenuInstance)
	{
		InGameMenuInstance = CreateWidget<UUserWidget>(PC, InGameMenuClass);
	}

	if (InGameMenuInstance->IsInViewport())
	{
		InGameMenuInstance->RemoveFromParent();

		PC->SetInputMode(FInputModeGameOnly());
		PC->SetShowMouseCursor(false);
	}
	else
	{
		InGameMenuInstance->AddToViewport();

		FInputModeGameAndUI InputMode;
		InputMode.SetWidgetToFocus(InGameMenuInstance->TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

		PC->SetInputMode(InputMode);
		PC->SetShowMouseCursor(true);
	}
}

void ANecPlayerCharacter::Interact()
{
	// Temp Function
	
	if (!CameraComponent)
	{
		return;
	}

	FVector Start = CameraComponent->GetComponentLocation();
	FVector ForwardVector = CameraComponent->GetForwardVector();
	float InteractionDistance = 500.0f;
	FVector End = Start + (ForwardVector * InteractionDistance);
	
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		ECC_Visibility,
		Params
	);

	DrawDebugLine(GetWorld(), 
		Start, 
		End, 
		bHit ? FColor::Green : FColor::Red, 
		false, 
		2.0f
	);

	if (bHit && HitResult.GetActor())
	{
		AActor* HitActor = HitResult.GetActor();
		UPrimitiveComponent* HitComponent = HitResult.GetComponent();

		bool bIsTarget = false;

		if (HitActor->GetName().Contains(TEXT("SM_Torture_Devices_Cage_Open")))
		{
			bIsTarget = true;
		}
		else if (UStaticMeshComponent* MeshComp = Cast<UStaticMeshComponent>(HitComponent))
		{
			if (MeshComp->GetStaticMesh() && MeshComp->GetStaticMesh()->GetName() == TEXT("SM_Torture_Devices_Cage_Open"))
			{
				bIsTarget = true;
			}
		}

		if (bIsTarget)
		{
			if (APlayerController* PC = Cast<APlayerController>(GetController()))
			{
				UKismetSystemLibrary::QuitGame(this, PC, EQuitPreference::Quit, false);
			}
		}
	}
}

void ANecPlayerCharacter::HandleDeath()
{
	if (HasAuthority())
	{		
		AController* CurrentController = GetController();

		if (CurrentController)
		{
			CurrentController->UnPossess();			
		}

		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	}
}

void ANecPlayerCharacter::LinkPlayerStateComponents()
{
	ANecPlayerState* PS = GetPlayerState<ANecPlayerState>();
	if (PS)
	{
		StatComponent = PS->GetStatComponent();
		StaminaComponent = PS->GetStaminaComponent();

		if (StatComponent && StaminaComponent)
		{
			if (HasAuthority())
			{ 
				StatComponent->BindToOwnerPawn(this);

				StatComponent->OnDeath.RemoveDynamic(this, &ANecPlayerCharacter::HandleDeath);
				StatComponent->OnDeath.AddDynamic(this, &ANecPlayerCharacter::HandleDeath);

				UE_LOG(LogTemp, Warning, TEXT("[Server] Successfully linked Component: %s"), *GetName());
			}
		}
	}
}

FGenericTeamId ANecPlayerCharacter::GetGenericTeamId() const
{
	return FGenericTeamId(TEAM_ID_PLAYER);
}

void ANecPlayerCharacter::SetLockOn(bool bEnable)
{
	if (bEnable)
	{
		GetCharacterMovement()->bOrientRotationToMovement = false;
		bUseControllerRotationYaw = true;
	}
	else
	{		   
		GetCharacterMovement()->bOrientRotationToMovement = true;
		bUseControllerRotationYaw = false;

		bUseControllerRotationPitch = false;
		bUseControllerRotationRoll = false;
	}

}

void ANecPlayerCharacter::Server_SetSprint_Implementation(bool bIsSprinting)
{
	if (!IsValid(PlayerMovementComponent) || !IsValid(StaminaComponent))
	{
		return;
	}

	if (bIsSprinting)
	{
		if (!StaminaComponent->IsExhausted() && StaminaComponent->GetCurrentStamina() > 0.0f)
		{
			GetCharacterMovement()->MaxWalkSpeed = PlayerMovementComponent->GetSprintSpeed();
			StaminaComponent->StartStaminaDrain(10.0f);
		}
		else
		{
			GetCharacterMovement()->MaxWalkSpeed = PlayerMovementComponent->GetNormalSpeed();
			StaminaComponent->StopStaminaDrain(false);
		}
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = PlayerMovementComponent->GetNormalSpeed();
		bool bResetExhaustion = StaminaComponent->GetCurrentStamina() > 0.0f;
		StaminaComponent->StopStaminaDrain(bResetExhaustion);
	}
}