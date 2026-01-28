// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterStatComponent.h"

#include "EnvironmentQuery/EnvQueryTypes.h"

UMonsterStatComponent::UMonsterStatComponent()
{
	
}

void UMonsterStatComponent::BeginPlay()
{
	Super::BeginPlay();
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
