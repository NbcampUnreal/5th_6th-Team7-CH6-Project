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

	// 이 공격에 사용할 몽타주
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimMontage* AttackMontage = nullptr;

	// 기본 공격력에 곱할 배율 (1타: 1.0, 2타: 1.2, 3타: 1.5 등)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.1"))
	float DamageMultiplier = 1.0f;

	// 공격 범위 배율
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.1"))
	float RangeMultiplier = 1.0f;

	// 다음 콤보 입력 가능 시작 시점 (몽타주 진행률 0.0 ~ 1.0)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float ComboWindowStart = 0.5f;

	// 다음 콤보 입력 가능 종료 시점
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float ComboWindowEnd = 0.9f;
};


UCLASS(BlueprintType)
class NECROMANCER_API UMonsterComboDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	// 콤보 시퀀스 (순서대로 실행)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combo")
	TArray<FComboAttackData> ComboSequence;

	// 콤보 리셋 시간 (이 시간 동안 공격 안 하면 1타로 초기화)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combo")
	float ComboResetTime = 2.0f;

	// 콤보 최대 횟수 (0이면 ComboSequence 크기 사용)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combo")
	int32 MaxComboCount = 0;

	// 헬퍼 함수
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