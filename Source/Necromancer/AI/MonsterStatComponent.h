// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Component/StatComponent.h"
#include "Curves/CurveFloat.h"
#include "MonsterStatComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NECROMANCER_API UMonsterStatComponent : public UStatComponent
{
	GENERATED_BODY()

public:
	UMonsterStatComponent();

	UFUNCTION(BlueprintCallable)
	float GetPoiseDamage() const { return PoiseDamage * CurrentPoiseDamageMultiplier; }

	UFUNCTION(BlueprintCallable)
	float GetAttackPower() const;

	// 현재 공격의 데미지 배율 설정/조회 (BTTask에서 공격 전 호출)
	UFUNCTION(BlueprintCallable)
	void SetDamageMultiplier(float NewMultiplier);
	UFUNCTION(BlueprintCallable)
	void ResetDamageMultiplier();

	// 현재 공격의 포이즈 배율 설정/조회 (BTTask에서 공격 전 호출)
	UFUNCTION(BlueprintCallable)
	void SetPoiseDamageMultiplier(float NewMultiplier);
	UFUNCTION(BlueprintCallable)
	void ResetPoiseDamageMultiplier();

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

	// 스킬 레벨 (0~1, 리액션 확률 조절)
	UFUNCTION(BlueprintCallable)
	float GetSkillLevel() const;

	// 층별 스탯 스케일링 적용 (서버 전용)
	UFUNCTION(BlueprintCallable, Category = "Stat|FloorScaling")
	void ApplyFloorScaling(int32 FloorLevel);

protected:
	virtual void BeginPlay() override;

private:
	// 공격
	// 포이즈 데미지 (플레이어에게 주는 포이즈)
	UPROPERTY(EditDefaultsOnly, Category = "Poise")
	float PoiseDamage = 30.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Stat")
	float AttackPower = 50.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Stat")
	float AttackRange = 150.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Stat")
	float DetectRange = 200.0f;

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

	// 현재 공격의 데미지 배율 (매 공격마다 BTTask에서 설정, 기본 1.0)
	float CurrentDamageMultiplier = 1.0f;

	// 현재 공격의 포이즈 배율 (매 공격마다 BTTask에서 설정, 기본 1.0)
	float CurrentPoiseDamageMultiplier = 1.0f;

	// 스킬 레벨 (0~1, 높을수록 회피/블록 확률 증가)
	UPROPERTY(EditDefaultsOnly, Category = "Stat", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float SkillLevel = 0.5f;

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