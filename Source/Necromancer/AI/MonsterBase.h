// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h" 
#include "MonsterBase.generated.h"

class UMonsterStatComponent;
class UBehaviorTree;
struct FItemData;

DECLARE_DELEGATE(FOnNextComboRequested);

UCLASS()
class NECROMANCER_API AMonsterBase : public ACharacter ,public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	
	AMonsterBase();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual FGenericTeamId GetGenericTeamId() const override;
	
	
	// 콤보 공격 시 다음 콤보 요청 델리게이트
	FOnNextComboRequested OnNextComboRequested;

	UFUNCTION(BlueprintCallable,Category="RVO")
	void SetRVOAvoidanceEnabled(bool bEnable);

	// 몽타주를 전 클라이언트에서 재생
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayMontage(UAnimMontage* Montage);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayDeathMontage();

	UFUNCTION(BlueprintCallable)
	bool GetIsDead();

	// 공격 상태 강제 정리
	UFUNCTION(BlueprintCallable, Category = "AI")
	void ForceCleanupAttackState();

	void RestoreMovementIfAlive();

	// 전투/순찰 이동 모드 전환
	// 전투: 타겟을 바라보면서 옆걸음 (Strafe)
	// 순찰: 이동 방향으로 몸 회전
	UFUNCTION(BlueprintCallable, Category = "AI")
	void SetCombatMovementMode(bool bCombat);

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

	// 경직 처리
	UFUNCTION()
	virtual void OnStagger();

	// 기절 처리
	UFUNCTION()
	virtual void OnStun();

	// 사망 처리 (Server Only)
	UFUNCTION()
	virtual void OnDeath();

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> DeathMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> HitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> StunMontage;

	// 물리 래그돌 전환
	void StartRagdoll();

	// 사망 시 아이템 스폰 (Server Only)
	void SpawnDropItems();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drop")
	TArray<FName> DropItemIDs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drop")
	float DropSpreadRadius = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RVO")
	float AvoidanceRadius = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RVO")
	float AvoidanceWeight = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MovementAcceleration = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MovementDeceleration = 300.0f;

	// 전투 중 타겟을 바라보는 회전 속도 (도/초)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float CombatRotationSpeed = 360.0f;

	// 피격 리액션 처리
	UFUNCTION()
	virtual void OnDamageReceived(float DamageAmount, FVector HitLocation);

	void OnHitReactMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	// 피격 시 공격 슬롯 없으면 임시 부여
	void TryGrantTemporarySlot();

	// 블로킹 상태 (방패 막기)
	UPROPERTY(ReplicatedUsing = OnRep_IsBlocking)
	bool bIsBlocking = false;

	UFUNCTION()
	void OnRep_IsBlocking();

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> BlockReactMontage;

public:
	// 방패 피해 감소율 (0.0 ~ 1.0, 0.8 = 80% 감소)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float ShieldGuardRate = 0.8f;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void SetBlockingState(bool bBlock);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool IsBlocking() const { return bIsBlocking; }
};
