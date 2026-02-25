// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MonsterAttackSetData.generated.h"

USTRUCT(BlueprintType)
struct FMonsterAttackEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> AttackMontage = nullptr;

	// 이 공격이 유효한 최소 거리
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0"))
	float MinDistance = 0.0f;

	// 이 공격이 유효한 최대 거리
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0"))
	float MaxDistance = 300.0f;

	// 가중치 (높을수록 선택 확률 증가)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.1"))
	float Weight = 1.0f;
};

UCLASS(BlueprintType)
class NECROMANCER_API UMonsterAttackSetData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	TArray<FMonsterAttackEntry> Attacks;

	// 거리에 맞는 공격 중 가중치 랜덤으로 하나 선택
	UAnimMontage* SelectAttackByDistance(float Distance) const;
};
