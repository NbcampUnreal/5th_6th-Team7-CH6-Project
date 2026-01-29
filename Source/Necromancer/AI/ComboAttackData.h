// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ComboAttackData.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FComboAttackData
{
	GENERATED_BODY()

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimMontage* AttackMontage = nullptr;

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.1"))
	float DamageMultiplier = 1.0f;

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.1"))
	float RangeMultiplier = 1.0f;

	
};


UCLASS(BlueprintType)
class NECROMANCER_API UMonsterComboDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combo")
	TArray<FComboAttackData> ComboSequence;

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combo")
	float ComboResetTime = 2.0f;

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combo")
	int32 MaxComboCount = 0;

	
	int32 GetMaxComboCount() const
	{
		return MaxComboCount > 0 ? MaxComboCount : ComboSequence.Num();
	}

	const FComboAttackData* GetComboData(int32 Index) const
	{
		if (ComboSequence.IsValidIndex(Index))
		{
			return &ComboSequence[Index];
		}
		return nullptr;
	}
};