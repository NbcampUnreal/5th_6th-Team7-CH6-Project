// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "ActiveGameplayEffectHandle.h"
#include "GA_CombatPersistenceBuff.generated.h"

/**
 * 전투 지속 버프 Passive Ability
 * - 전투 상태(Monster.State.Combat)가 10초 지속되면 공격력 버프 적용
 * - 전투 종료 후 3초 뒤에 버프 해제
 * - 전투 재돌입 시 버프 해제 타이머 취소
 */
UCLASS()
class NECROMANCER_API UGA_CombatPersistenceBuff : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_CombatPersistenceBuff();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	// 버프 GameplayEffect
	UPROPERTY(EditDefaultsOnly, Category = "Buff")
	TSubclassOf<UGameplayEffect> BuffEffectClass;

	// 전투 시작 후 버프 적용까지 대기 시간 (초)
	UPROPERTY(EditDefaultsOnly, Category = "Buff")
	float BuffActivationDelay = 10.0f;

	// 전투 종료 후 버프 해제까지 대기 시간 (초)
	UPROPERTY(EditDefaultsOnly, Category = "Buff")
	float BuffRemovalDelay = 3.0f;

private:
	// 타이머 핸들
	FTimerHandle ActivationTimerHandle;
	FTimerHandle RemovalTimerHandle;

	// 활성화된 버프 핸들
	FActiveGameplayEffectHandle ActiveBuffHandle;

	// 태그 변화 델리게이트 핸들
	FDelegateHandle TagChangedDelegateHandle;

	// Combat 태그 변화 콜백
	void OnCombatTagChanged(const FGameplayTag Tag, int32 NewCount);

	// 전투 상태 진입/이탈 처리
	void OnCombatEntered();
	void OnCombatExited();

	// 버프 적용/제거
	void ApplyBuff();
	void RemoveBuff();
};
