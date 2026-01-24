// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_MonsterDeath.generated.h"

/**
 * 몬스터 사망 처리 Ability
 * - 사망 애니메이션 재생
 * - 애니메이션 완료 후 Ragdoll 물리화
 * - 일정 시간 후 액터 제거
 */
UCLASS()
class NECROMANCER_API UGA_MonsterDeath : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_MonsterDeath();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	// 사망 애니메이션 몽타주
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> DeathMontage;

	// 액터 제거까지 대기 시간
	UPROPERTY(EditDefaultsOnly, Category = "Death")
	float DeathRemovalTime = 5.0f;

private:
	void OnDeathMontageCompleted(UAnimMontage* Montage, bool bInterrupted);
	void ActivateRagdoll();
};
