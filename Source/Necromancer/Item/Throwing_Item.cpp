//Throwing_Item.cpp

#include "Item/Throwing_Item.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"

AThrowing_Item::AThrowing_Item()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	SetReplicateMovement(true);

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	SetRootComponent(Collision);
	Collision->InitSphereRadius(15.f);
	Collision->SetNotifyRigidBodyCollision(true);
	Collision->OnComponentHit.AddDynamic(this, &AThrowing_Item::OnHit);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = ThrowSpeed;
	ProjectileMovement->MaxSpeed = ThrowSpeed;
	ProjectileMovement->bRotationFollowsVelocity = true;
}

void AThrowing_Item::ExecuteUse(ACharacter* User)
{
	if (!HasAuthority())
	{
		Server_ExecuteUse(User);
		return;
	}

	if (!User) return;

	FVector SpawnLocation = User->GetActorLocation() + User->GetActorForwardVector() * 100.f;
	FRotator SpawnRotation = User->GetControlRotation();

	SetActorLocation(SpawnLocation);
	SetActorRotation(SpawnRotation);

	ProjectileMovement->Velocity = SpawnRotation.Vector() * ThrowSpeed;
}

void AThrowing_Item::Server_ExecuteUse_Implementation(ACharacter* User)
{
	ExecuteUse(User);
}

void AThrowing_Item::OnHit(UPrimitiveComponent* HitComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	FVector NormalImpulse,
	const FHitResult& Hit)
{
	if (!HasAuthority()) return;

	Multicast_OnHitFX(Hit.ImpactPoint);

	Destroy();
}

void AThrowing_Item::Multicast_OnHitFX_Implementation(FVector Location)
{
}
