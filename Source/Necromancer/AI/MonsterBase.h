// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/Character.h"
#include "MonsterBase.generated.h"

class UMonsterStatComponent;
class UBehaviorTree;
class UHealthComponent;
UCLASS()
class NECROMANCER_API AMonsterBase : public ACharacter ,public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	
	AMonsterBase();
	virtual FGenericTeamId GetGenericTeamId() const override;

protected:
	
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<UHealthComponent> HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<UMonsterStatComponent> MonsterStatComponent;
	
	UFUNCTION()
	void OnStagger();

	UFUNCTION()
	void OnStun();
	
	UFUNCTION()
	void OnDeath();
	
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> DeathMontage;
	
	void StartRagdoll();


	
};
