// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "MonsterStatComponent.generated.h"

// 경직
DECLARE_MULTICAST_DELEGATE(FOnStagger);
// 기절
DECLARE_MULTICAST_DELEGATE(FOnStun);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NECROMANCER_API UMonsterStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMonsterStatComponent();

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

	// 등급
	UPROPERTY(EditDefaultsOnly, Category = "Stat")
	FGameplayTag MonsterGrade;
};