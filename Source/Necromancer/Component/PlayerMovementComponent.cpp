#include "Component/PlayerMovementComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UPlayerMovementComponent::UPlayerMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);

	SprintSpeed = NormalSpeed * SprintSpeedMultiplier;
}

void UPlayerMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ACharacter>(GetOwner());

	SprintSpeed = NormalSpeed * SprintSpeedMultiplier;
}

void UPlayerMovementComponent::ProcessMove(const FVector2D& Value)
{
	if (IsValid(OwnerCharacter) && OwnerCharacter->GetController())
	{
		const FRotator Rotation = OwnerCharacter->GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		OwnerCharacter->AddMovementInput(ForwardDirection, Value.Y);
		OwnerCharacter->AddMovementInput(RightDirection, Value.X);
	}
}

void UPlayerMovementComponent::ProcessLook(const FVector2D& Value)
{
	if (IsValid(OwnerCharacter) && OwnerCharacter->GetController())
	{
		OwnerCharacter->AddControllerYawInput(Value.X);
		OwnerCharacter->AddControllerPitchInput(Value.Y);
	}
}

void UPlayerMovementComponent::SetSprint(bool bShouldSprint)
{
	if (IsValid(OwnerCharacter))
	{
		float NewSpeed = bShouldSprint ? SprintSpeed : NormalSpeed;
		OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed = NewSpeed;

		bIsSprinting = bShouldSprint ? true : false;
	}
}