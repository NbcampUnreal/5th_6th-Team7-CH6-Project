// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterStatComponent.h"

UMonsterStatComponent::UMonsterStatComponent()
{
	SetIsReplicatedByDefault(true);
}

void UMonsterStatComponent::BeginPlay()
{
	Super::BeginPlay();
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	BindToOwnerPawn(OwnerPawn);
}

void UMonsterStatComponent::ApplyPoise(float Amount)
{
	if (!GetOwner()->HasAuthority())
	{
		return;
	}

	// 단일 경직 체크
	if (Amount >= SinglePoiseThreshold)
	{
		OnStagger.Broadcast();
	}

	// 누적
	CurrentPoise += Amount;

	// 기절 체크
	if (CurrentPoise >= MaxPoise)
	{
		OnStun.Broadcast();
		CurrentPoise = 0.0f;
	}
}

float UMonsterStatComponent::GetAttackPower() const
{
	return AttackPower;
}

float UMonsterStatComponent::GetAttackRange() const
{
	return AttackRange;
}

float UMonsterStatComponent::GetDetectRange() const
{
	return DetectRange;
}

float UMonsterStatComponent::GetProjectileSpeed() const
{
	return ProjectileSpeed;
}

float UMonsterStatComponent::GetProjectileGravityScale() const
{
	return ProjectileGravityScale;
}

float UMonsterStatComponent::GetMinAttackRange() const
{
	return MinAttackRange;
}

bool UMonsterStatComponent::GetIsRanged() const
{
	return bIsRanged;
}

bool UMonsterStatComponent::CanAttack() const
{
	const UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}
	return (World->GetTimeSeconds() - LastAttackTime) >= AttackCooldown;
}

void UMonsterStatComponent::MarkAttackUsed()
{
	if (const UWorld* World = GetWorld())
	{
		LastAttackTime = World->GetTimeSeconds();
	}
}

float UMonsterStatComponent::GetAttackCooldown() const
{
	return AttackCooldown;
}
