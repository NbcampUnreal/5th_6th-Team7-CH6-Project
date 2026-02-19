#include "Character/Animation/NecAnimInstanceBase.h"
#include "Character/NecPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UNecAnimInstanceBase::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwnerCharacter = Cast<ANecPlayerCharacter>(GetOwningActor());
	if (IsValid(OwnerCharacter))
	{
		OwnerCharacterMovementComponent = OwnerCharacter->GetCharacterMovement();
	}
}

void UNecAnimInstanceBase::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!IsValid(OwnerCharacter) || !IsValid(OwnerCharacterMovementComponent))
	{
		return;
	}

	Velocity = OwnerCharacterMovementComponent->Velocity;
	GroundSpeed = FVector(Velocity.X, Velocity.Y, 0.f).Size();

	if (GroundSpeed > 3.0f)
	{
		FRotator ActorRotation = OwnerCharacter->GetActorRotation();
		FVector LocalVelocity = ActorRotation.UnrotateVector(Velocity);
		Direction = LocalVelocity.Rotation().Yaw;
	}
	else
	{
		Direction = 0.0f;
	}

	//bShouldMove = (!OwnerCharacterMovementComponent->GetCurrentAcceleration().IsNearlyZero()) && (5.0f < GroundSpeed);
	bShouldMove = GroundSpeed > 5.0f;
	bIsFalling = OwnerCharacterMovementComponent->IsFalling();
}