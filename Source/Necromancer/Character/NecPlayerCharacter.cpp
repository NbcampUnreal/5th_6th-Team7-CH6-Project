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

ANecPlayerCharacter::ANecPlayerCharacter()
{
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
}

void ANecPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
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
					ETriggerEvent::Triggered,
					this,
					&ANecPlayerCharacter::Attack
				);
			}

			if (PlayerController->GuardAction)
			{
				EnhancedInputComp->BindAction(
					PlayerController->GuardAction,
					ETriggerEvent::Triggered,
					this,
					&ANecPlayerCharacter::Guard
				);
			}

			if (PlayerController->LockOnAction)
			{
				EnhancedInputComp->BindAction(
					PlayerController->LockOnAction,
					ETriggerEvent::Triggered,
					this,
					&ANecPlayerCharacter::LockOn
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
	if (IsValid(PlayerMovementComponent))
	{
		PlayerMovementComponent->ProcessLook(Value.Get<FVector2D>());
	}
}

void ANecPlayerCharacter::StartSprint(const FInputActionValue& Value)
{
	if (IsValid(PlayerMovementComponent))
	{
		PlayerMovementComponent->SetSprint(true);
		Server_SetSprint(true);
	}
}

void ANecPlayerCharacter::StopSprint(const FInputActionValue& Value)
{
	if (IsValid(PlayerMovementComponent))
	{
		PlayerMovementComponent->SetSprint(false);
		Server_SetSprint(false);
	}
}

void ANecPlayerCharacter::Attack(const FInputActionValue& Value)
{

}

void ANecPlayerCharacter::Guard(const FInputActionValue& Value)
{

}

void ANecPlayerCharacter::LockOn(const FInputActionValue& Value)
{

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
			UE_LOG(LogTemp, Warning, TEXT("Successfully linked PlayerState\'s Components: %s"), *GetName());
		}
	}
}

FGenericTeamId ANecPlayerCharacter::GetGenericTeamId() const
{
	return FGenericTeamId(TEAM_ID_PLAYER);
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
