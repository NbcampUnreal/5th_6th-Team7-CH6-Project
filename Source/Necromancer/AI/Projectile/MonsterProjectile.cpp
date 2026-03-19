// Fill out your copyright notice in the Description page of Project Settings.

#include "MonsterProjectile.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GenericTeamAgentInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Necromancer.h"
#include "DamageType/NecDamageType.h"

AMonsterProjectile::AMonsterProjectile()
{
	SphereComponent = CreateDefaultSubobject<USphereComponent>("SphereComponent");
	RootComponent = SphereComponent;
	SphereComponent->SetSphereRadius(20.0f);

	LoopNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>("LoopNiagaraComponent");
	LoopNiagaraComponent->SetupAttachment(SphereComponent);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovementComponent");
	ProjectileMovementComponent->InitialSpeed = 2000.0f;
	ProjectileMovementComponent->ProjectileGravityScale = 0.0f;

	bReplicates = true;
}

void AMonsterProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	SphereComponent->OnComponentHit.AddDynamic(this, &AMonsterProjectile::OnActorHit);
}

void AMonsterProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMonsterProjectile, ProjectileDamage);
}

void AMonsterProjectile::InitProjectile(float InDamage, float InSpeed, float InGravityScale)
{
	ProjectileDamage = InDamage;

	if (GetOwner())
	{
		SphereComponent->IgnoreActorWhenMoving(GetOwner(), true);
	}
	if (GetInstigator())
	{
		SphereComponent->IgnoreActorWhenMoving(GetInstigator(), true);
	}

	if (ProjectileMovementComponent)
	{
		ProjectileMovementComponent->InitialSpeed = InSpeed;
		ProjectileMovementComponent->MaxSpeed = InSpeed;
		ProjectileMovementComponent->ProjectileGravityScale = InGravityScale;
		ProjectileMovementComponent->Velocity = GetActorForwardVector() * InSpeed;
	}
}

void AMonsterProjectile::OnActorHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor == GetOwner() || OtherActor == GetInstigator())
	{
		return;
	}

	// Server Only
	if (HasAuthority())
	{
		UGameplayStatics::ApplyDamage(OtherActor, ProjectileDamage, GetInstigatorController(), this, UNecDamageType::StaticClass());
	}

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ExplosionEffect, GetActorLocation());

	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation());
	}

	if (HasAuthority())
	{
		Destroy();
	}
}
