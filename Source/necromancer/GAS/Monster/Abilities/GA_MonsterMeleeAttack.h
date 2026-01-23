// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_MonsterMeleeAttack.generated.h"

/**
 * 
 */
UCLASS()
class NECROMANCER_API UGA_MonsterMeleeAttack : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	
	UGA_MonsterMeleeAttack();
	
	//어빌리티 작동
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	//어빌리티 끝
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	//공격 판정 애님 노티파이에서 호출
	UFUNCTION(BlueprintCallable,Category="Combat")
	void PerformAttackTrace();
	
protected:
	
	//공격 애니메이션 몽타주
	UPROPERTY(EditDefaultsOnly, Category="Animation")
	TObjectPtr<UAnimMontage> AttackMontage;
	
	//데미지 GE
	UPROPERTY(EditDefaultsOnly, Category="Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;
	
	//공격 판정 거리
	UPROPERTY(EditDefaultsOnly, Category="Trace")
	float AttackTraceDistance = 150.0f;
	
	//공격 판정 반지름
	UPROPERTY(EditDefaultsOnly, Category="Trace")
	float AttackTraceRadius = 50.0f;
	
private:
	
	UFUNCTION()
	void OnMontageCompleted(UAnimMontage* Montage, bool bInterrupted);
	
	void ApplyDamageToTarget(AActor* Target);
};
