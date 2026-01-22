#include "GAS/Character/NecPlayerCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "necromancer.h"
#include "Game/NecPlayerState.h"
#include "AbilitySystemComponent.h"
#include "GAS/Character/CharacterAttributeSet.h"

ANecPlayerCharacter::ANecPlayerCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
}

void ANecPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitAbilityActorInfo();

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);

		if (HasAuthority())
		{
			for (const TSubclassOf<UGameplayAbility>& AbilityClass : DefaultAbilities)
			{
				if (AbilityClass)
				{
					FGameplayAbilitySpec Spec(AbilityClass, 1, -1, this);
					AbilitySystemComponent->GiveAbility(Spec);
				}
			}
		}

		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthAttribute()).AddUObject(this, &ANecPlayerCharacter::OnHealthChangedCallback);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetStaminaAttribute()).AddUObject(this, &ANecPlayerCharacter::OnStaminaChangedCallback);
	}
}

void ANecPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	InitAbilityActorInfo();
}

void ANecPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ANecPlayerCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &ANecPlayerCharacter::Look);

		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ANecPlayerCharacter::Look);
	}
	else
	{
		UE_LOG(Lognecromancer, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ANecPlayerCharacter::InitAbilityActorInfo()
{
	ANecPlayerState* PS = GetPlayerState<ANecPlayerState>();
	if (PS)
	{
		AbilitySystemComponent = PS->GetAbilitySystemComponent();
		AbilitySystemComponent->InitAbilityActorInfo(PS, this);
	}
}

void ANecPlayerCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	DoMove(MovementVector.X, MovementVector.Y);
}

void ANecPlayerCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void ANecPlayerCharacter::Attack(const FInputActionValue& Value)
{

}

void ANecPlayerCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void ANecPlayerCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void ANecPlayerCharacter::DoJumpStart()
{
	Jump();
}

void ANecPlayerCharacter::DoJumpEnd()
{
	StopJumping();
}

void ANecPlayerCharacter::OnHealthChangedCallback(const FOnAttributeChangeData& Data) const
{
	OnHealthChanged.Broadcast(Data.NewValue, AttributeSet->GetMaxHealth());
}

void ANecPlayerCharacter::OnStaminaChangedCallback(const FOnAttributeChangeData& Data) const
{
	OnHealthChanged.Broadcast(Data.NewValue, AttributeSet->GetMaxStamina());
}

void ANecPlayerCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();
	
}