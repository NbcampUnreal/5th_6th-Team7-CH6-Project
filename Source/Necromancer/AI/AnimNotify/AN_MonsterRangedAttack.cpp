// Fill out your copyright notice in the Description page of Project Settings.

#include "AN_MonsterRangedAttack.h"
#include "MonsterStatComponent.h"
#include "Projectile/MonsterProjectile.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Necromancer.h"

void UAN_MonsterRangedAttack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	AActor* OwnerActor = MeshComp->GetOwner();
	if (!OwnerActor || !OwnerActor->HasAuthority())
	{
		return;
	}

	if (!ProjectileClass)
	{
		return;
	}

	UMonsterStatComponent* StatComp = OwnerActor->FindComponentByClass<UMonsterStatComponent>();
	if (!StatComp)
	{
		return;
	}

	
	APawn* OwnerPawn = Cast<APawn>(OwnerActor);
	if (!OwnerPawn)
	{
		return;
	}

	AAIController* AIC = Cast<AAIController>(OwnerPawn->GetController());
	if (!AIC)
	{
		return;
	}

	const UBlackboardComponent* BB = AIC->GetBlackboardComponent();
	if (!BB)
	{
		return;
	}

	AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(FName(NAME_TargetActor)));
	if (!TargetActor)
	{
		return;
	}

	
	FVector SpawnLocation = MeshComp->DoesSocketExist(MuzzleSocketName)? MeshComp->GetSocketLocation(MuzzleSocketName): OwnerActor->GetActorLocation();

	
	FVector TargetLocation = TargetActor->GetActorLocation();

	if (bUsePrediction && PredictionAccuracy > 0.0f)
	{
		FVector TargetVelocity = TargetActor->GetVelocity();
		float ProjectileSpeed = StatComp->GetProjectileSpeed();
		float Distance = FVector::Dist(SpawnLocation, TargetLocation);
		float TimeToTarget = Distance / ProjectileSpeed;

		
		TargetLocation += TargetVelocity * TimeToTarget * PredictionAccuracy;
	}

	
	FVector Direction = (TargetLocation - SpawnLocation).GetSafeNormal();
	FRotator SpawnRotation = Direction.Rotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = OwnerActor;
	SpawnParams.Instigator = OwnerPawn;

	AMonsterProjectile* Projectile = OwnerActor->GetWorld()->SpawnActor<AMonsterProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);

	if (Projectile)
	{
		Projectile->InitProjectile(StatComp->GetAttackPower(),StatComp->GetProjectileSpeed(),StatComp->GetProjectileGravityScale());
	}
}
