// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h" 
#include "MonsterBase.generated.h"

class UMonsterStatComponent;
class UBehaviorTree;

DECLARE_DELEGATE(FOnNextComboRequested);

UCLASS()
class NECROMANCER_API AMonsterBase : public ACharacter ,public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	
	AMonsterBase();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual FGenericTeamId GetGenericTeamId() const override;
	
	
	FOnNextComboRequested OnNextComboRequested;
	
	UFUNCTION(BlueprintCallable,Category="RVO")
	void SetRVOAvoidanceEnabled(bool bEnable);
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayMontage(UAnimMontage* Montage);
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayDeathMontage();
	
	UFUNCTION(BlueprintCallable)
	bool GetIsDead();

protected:
	
	virtual void BeginPlay() override;
	
	UPROPERTY(ReplicatedUsing = OnRep_IsDead)
	bool bIsDead = false;
	
	UFUNCTION()
	void OnRep_IsDead();
	
	UPROPERTY()
	FTimerHandle DeathTimerHandle;
	
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
	
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> HitReactMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> StunMontage;
	
	void StartRagdoll();
	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RVO")
	float AvoidanceRadius = 100.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RVO")
	float AvoidanceWeight = 0.5f;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MovementAcceleration = 300.0f;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MovementDeceleration = 300.0f;
	
	UFUNCTION()
	void OnDamageReceived(float DamageAmount, FVector HitLocation);


	
};
