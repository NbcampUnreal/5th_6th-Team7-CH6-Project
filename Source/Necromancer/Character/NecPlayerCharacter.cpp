#include "Character/NecPlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Controller/NecPlayerController.h"
#include "EnhancedInputComponent.h"

ANecPlayerCharacter::ANecPlayerCharacter()
{
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	SprintSpeed = NormalSpeed * SprintSpeedMultiplier;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 300.0f;
	SpringArmComp->bUsePawnControlRotation = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;	
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

			if (PlayerController->LockonAction)
			{
				EnhancedInputComp->BindAction(
					PlayerController->LockonAction,
					ETriggerEvent::Triggered,
					this,
					&ANecPlayerCharacter::Lockon
				);
			}
		}
	}	
}

void ANecPlayerCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{		
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
				
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);		
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
				
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ANecPlayerCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{		
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ANecPlayerCharacter::StartSprint(const FInputActionValue& Value)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
		Server_SetMaxWalkSpeed(SprintSpeed);
	}	
}

void ANecPlayerCharacter::StopSprint(const FInputActionValue& Value)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
		Server_SetMaxWalkSpeed(NormalSpeed);
	}
}

void ANecPlayerCharacter::Attack(const FInputActionValue& Value)
{

}

void ANecPlayerCharacter::Guard(const FInputActionValue& Value)
{

}

void ANecPlayerCharacter::Lockon(const FInputActionValue& Value)
{

}

void ANecPlayerCharacter::Server_SetMaxWalkSpeed_Implementation(float NewSpeed)
{
	GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
}