// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterProjectile.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AMonsterProjectile::AMonsterProjectile()
{
	SphereComponent = CreateDefaultSubobject<USphereComponent>("SphereComponent");
	RootComponent = SphereComponent;
	SphereComponent->SetSphereRadius(20.0f);
	
	LoopNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>("LoopNiagaraComponent");
	LoopNiagaraComponent -> SetupAttachment(SphereComponent);
	
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovementComponent");
	ProjectileMovementComponent->InitialSpeed = 2000.0f;
	ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
}

void AMonsterProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	SphereComponent->OnComponentHit.AddDynamic(this, &AMonsterProjectile::OnActorHit);
	
}

void AMonsterProjectile::OnActorHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	TSubclassOf<UDamageType> DamageTypeClass = UDamageType::StaticClass();
	
	UGameplayStatics::ApplyDamage(OtherActor, 10.0f, GetInstigatorController(),this, DamageTypeClass);
	
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ExplosionEffect,GetActorLocation());
	
	Destroy();
}



