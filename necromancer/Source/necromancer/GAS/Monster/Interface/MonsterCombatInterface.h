// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MonsterCombatInterface.generated.h"

/**
 * 
 */

UINTERFACE(MinimalAPI, BlueprintType)
class UMonsterCombatInterface : public UInterface
{
	GENERATED_BODY()
};


class NECROMANCER_API IMonsterCombatInterface
{
	GENERATED_BODY()

public:
	
	
	// 공격 판정 실행 
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat|Attack")
	void ExecuteAttackTrace(FName AttackBoneName);

	// 공격 시작 알림 
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat|Attack")
	void OnAttackStarted();

	// 공격 종료 알림
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat|Attack")
	void OnAttackEnded(bool bWasInterrupted);

	// 공격 가능 여부 확인
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat|Attack")
	bool CanAttack() const;
	
	// 사망 처리 
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat|Death")
	void HandleDeath();

	// 생존 여부 확인
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat|State")
	bool IsAlive() const;
};