// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Component/StatComponent.h"
#include "GameplayTagContainer.h"
#include "Curves/CurveFloat.h"
#include "MonsterStatComponent.generated.h"

// 경직
DECLARE_MULTICAST_DELEGATE(FOnStagger);
// 기절
DECLARE_MULTICAST_DELEGATE(FOnStun);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NECROMANCER_API UMonsterStatComponent : public UStatComponent
{
	GENERATED_BODY()

public:
	UMonsterStatComponent();

	// 강인도 누적 (경직/기절 판정)
	UFUNCTION(BlueprintCallable)
	void ApplyPoise(float Amount);

	// Delegate
	FOnStagger OnStagger;
	FOnStun OnStun;
	
	UFUNCTION(BlueprintCallable)
	float GetAttackPower() const;
	UFUNCTION(BlueprintCallable)
	float GetAttackRange() const;
	UFUNCTION(BlueprintCallable)
	float GetDetectRange() const;

	// 원거리 스탯
	UFUNCTION(BlueprintCallable)
	float GetProjectileSpeed() const;
	UFUNCTION(BlueprintCallable)
	float GetProjectileGravityScale() const;
	UFUNCTION(BlueprintCallable)
	float GetMinAttackRange() const;
	UFUNCTION(BlueprintCallable)
	bool GetIsRanged() const;

	// 공격 쿨타임 확인
	UFUNCTION(BlueprintCallable)
	bool CanAttack() const;

	// 공격 사용 기록 (쿨타임 시작)
	UFUNCTION(BlueprintCallable)
	void MarkAttackUsed();

	UFUNCTION(BlueprintCallable)
	float GetAttackCooldown() const;

	// 층별 스탯 스케일링 적용 (서버 전용)
	UFUNCTION(BlueprintCallable, Category = "Stat|FloorScaling")
	void ApplyFloorScaling(int32 FloorLevel);

protected:
	virtual void BeginPlay() override;

private:
	// 공격
	UPROPERTY(EditDefaultsOnly, Category = "Stat")
	float AttackPower = 50.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Stat")
	float AttackRange = 150.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Stat")
	float DetectRange = 200.0f;

	// 경직
	UPROPERTY(EditDefaultsOnly, Category = "Poise")
	float MaxPoise = 100.0f;

	float CurrentPoise = 0.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Poise")
	float SinglePoiseThreshold = 30.0f;

	// 원거리 전투
	UPROPERTY(EditDefaultsOnly, Category = "Stat|Ranged")
	float ProjectileSpeed = 2000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Stat|Ranged")
	float ProjectileGravityScale = 0.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Stat|Ranged")
	float MinAttackRange = 0.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Stat|Ranged")
	bool bIsRanged = false;

	// 공격 쿨타임
	UPROPERTY(EditDefaultsOnly, Category = "Stat|Cooldown")
	float AttackCooldown = 2.0f;

	float LastAttackTime = -999.0f;

	// 등급
	UPROPERTY(EditDefaultsOnly, Category = "Stat")
	FGameplayTag MonsterGrade;

	// 층별 스케일링 (X=Floor, Y=Multiplier)
	UPROPERTY(EditDefaultsOnly, Category = "Stat|FloorScaling")
	TObjectPtr<UCurveFloat> HealthScaleCurve;

	UPROPERTY(EditDefaultsOnly, Category = "Stat|FloorScaling")
	TObjectPtr<UCurveFloat> AttackScaleCurve;

	UPROPERTY(EditDefaultsOnly, Category = "Stat|FloorScaling")
	TObjectPtr<UCurveFloat> ArmorScaleCurve;

	UPROPERTY(EditDefaultsOnly, Category = "Stat|FloorScaling")
	TObjectPtr<UCurveFloat> PoiseScaleCurve;
};