// Fill out your copyright notice in the Description page of Project Settings.
#include "MonsterAttackSetData.h"

const FMonsterAttackEntry* UMonsterAttackSetData::SelectAttackByDistance(float Distance) const
{
	TArray<const FMonsterAttackEntry*> ValidAttacks;
	float TotalWeight = 0.0f;

	for (const FMonsterAttackEntry& Entry : Attacks)
	{
		if (!Entry.AttackMontage)
		{
			continue;
		}

		if (Distance >= Entry.MinDistance && Distance <= Entry.MaxDistance)
		{
			ValidAttacks.Add(&Entry);
			TotalWeight += Entry.Weight;
		}
	}

	if (ValidAttacks.Num() == 0 || TotalWeight <= 0.0f)
	{
		return nullptr;
	}

	// 가중치 랜덤 선택
	float RandomValue = FMath::FRandRange(0.0f, TotalWeight);
	float Accumulated = 0.0f;

	for (const FMonsterAttackEntry* Entry : ValidAttacks)
	{
		Accumulated += Entry->Weight;
		if (RandomValue <= Accumulated)
		{
			return Entry;
		}
	}

	return ValidAttacks.Last();
}
