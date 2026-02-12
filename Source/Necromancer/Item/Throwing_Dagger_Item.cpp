//Throwing_Dagger_Item.cpp

#include "Item/Throwing_Dagger_Item.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SphereComponent.h"

AThrowing_Dagger_Item::AThrowing_Dagger_Item()
{
	Damage = 25.f;
	bStickToTarget = true;
	LifeAfterHit = 5.f;
}

void AThrowing_Dagger_Item::OnHit(
	UPrimitiveComponent* HitComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	FVector NormalImpulse,
	const FHitResult& Hit)
{
	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);

	if (!HasAuthority())
		return;

	if (!OtherActor || OtherActor == this)
		return;

	HandleDamage(OtherActor);

	if (bStickToTarget && OtherComp)
	{
		HandleStick(OtherComp, Hit);
	}

	SetLifeSpan(LifeAfterHit);
}

void AThrowing_Dagger_Item::HandleDamage(AActor* OtherActor)
{
	UGameplayStatics::ApplyDamage(OtherActor,Damage,GetInstigatorController(),this,UDamageType::StaticClass());
}

void AThrowing_Dagger_Item::HandleStick(UPrimitiveComponent* OtherComp,const FHitResult& Hit)
{
	if (ProjectileMovement)
	{
		ProjectileMovement->StopMovementImmediately();
	}

	if (Collision)
	{
		Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	AttachToComponent(OtherComp, FAttachmentTransformRules::KeepWorldTransform);

	SetActorLocation(Hit.ImpactPoint);
}
