#include "Character/NecPlayerCharacter.h"

ANecPlayerCharacter::ANecPlayerCharacter()
{
 	PrimaryActorTick.bCanEverTick = false;

}

void ANecPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ANecPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ANecPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}