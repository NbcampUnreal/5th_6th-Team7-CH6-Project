// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "MonsterBase.h"
#include "MonsterCombatInterface.h"
#include "Nec_SkeletonMeleeMonster.generated.h"

class UGameplayAbility;

UCLASS()
class NECROMANCER_API ANec_SkeletonMeleeMonster : public AMonsterBase, public IMonsterCombatInterface
{
	GENERATED_BODY()

public:
	
	ANec_SkeletonMeleeMonster();
	
	
	
	//인터페이스 구현
	virtual void ExecuteAttackTrace_Implementation(FName AttackBoneName) override;
	virtual void OnAttackStarted_Implementation() override;
	virtual void OnAttackEnded_Implementation(bool bWasInterrupted) override;
	virtual bool CanAttack_Implementation() const override;
	virtual void HandleDeath_Implementation() override;
	virtual bool IsAlive_Implementation() const override;
	
	// StateTree에서 호출
	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool TryActivateMeleeAttack();
	
protected:
	
	virtual void BeginPlay() override;

	// 근접 공격 어빌리티 클래스
	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	TSubclassOf<UGameplayAbility> MeleeAttackAbilityClass;

	// 전투 지속 버프 어빌리티 클래스
	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	TSubclassOf<UGameplayAbility> CombatPersistenceBuffAbilityClass;

	// 사망 처리 어빌리티 클래스
	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	TSubclassOf<UGameplayAbility> DeathAbilityClass;

	// 공격 상태
	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	bool bIsAttacking = false;

	// 사망 상태
	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	bool bIsDead = false;
	
public:
	// 어빌리티 초기화
	void InitializeAbilities();
	
	
};
