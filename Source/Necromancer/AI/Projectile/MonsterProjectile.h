// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MonsterProjectile.generated.h"

class UNiagaraSystem;
class USphereComponent;
class UProjectileMovementComponent;
class UNiagaraComponent;
UCLASS()
class NECROMANCER_API AMonsterProjectile : public AActor
{
	GENERATED_BODY()

public:
	
	AMonsterProjectile();
	
	virtual void PostInitializeComponents() override;

protected:
	UPROPERTY(EditDefaultsOnly,Category ="Effect")
	TObjectPtr<UNiagaraSystem> ExplosionEffect;
	
	UPROPERTY(EditDefaultsOnly,Category ="Compomemts")
	TObjectPtr<USphereComponent> SphereComponent;
	
	UPROPERTY(EditDefaultsOnly,Category ="Compomemts")
	TObjectPtr<UNiagaraComponent> LoopNiagaraComponent;;
	
	UPROPERTY(EditDefaultsOnly,Category ="Compomemts")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;

	UFUNCTION()
	void OnActorHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
