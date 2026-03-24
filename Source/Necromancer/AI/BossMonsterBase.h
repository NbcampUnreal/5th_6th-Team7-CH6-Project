#pragma once

#include "CoreMinimal.h"
#include "MonsterBase.h"
#include "NiagaraSystem.h"
#include "Particles/ParticleSystem.h"
#include "BossMonsterBase.generated.h"

class UBossPhaseDataAsset;

UCLASS()
class NECROMANCER_API ABossMonsterBase : public AMonsterBase
{
	GENERATED_BODY()

public:
	ABossMonsterBase();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 순간이동 (BTTask에서 호출, 모든 클라이언트 동기화)
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Teleport(FVector NewLocation, FRotator NewRotation);

	UFUNCTION(BlueprintCallable, Category = "Boss")
	int32 GetCurrentPhase() const { return CurrentPhase; }

	UFUNCTION(BlueprintCallable, Category = "Boss")
	bool IsPhaseTransitioning() const { return bIsTransitioning; }

	// 슈퍼아머 제어 (BTTask_BossAction에서 호출)
	void SetSuperArmor(bool bEnable) { bHasSuperArmor = bEnable; }

	UFUNCTION(BlueprintCallable, Category = "Boss")
	bool HasSuperArmor() const { return bHasSuperArmor; }

	// 순찰 토글 (인게임에서 코드/BP로 제어)
	UFUNCTION(BlueprintCallable, Category = "Boss|Patrol")
	bool IsPatrolEnabled() const { return bPatrolEnabled; }

	UFUNCTION(BlueprintCallable, Category = "Boss|Patrol")
	void SetPatrolEnabled(bool bEnable) { bPatrolEnabled = bEnable; }

	UFUNCTION(BlueprintCallable, Category = "Boss|Patrol")
	float GetPatrolWalkSpeed() const { return PatrolWalkSpeed; }

protected:
	virtual void BeginPlay() override;
	virtual void OnDamageReceived(float DamageAmount, FVector HitLocation, bool bPoiseBroken) override;

	// 페이즈 전환 체크 (OnDamageReceived 후 호출)
	void CheckPhaseTransition();

	// 새 페이즈로 전환
	void TransitionToPhase(int32 NewPhase);

	// 페이즈 전환 시 호출 (서브클래스 확장 가능)
	virtual void OnPhaseChanged(int32 OldPhase, int32 NewPhase);

	// 페이즈 전환 몽타주 종료 콜백
	void OnTransitionMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	// 페이즈 설정 DataAsset
	UPROPERTY(EditDefaultsOnly, Category = "Boss")
	TObjectPtr<UBossPhaseDataAsset> PhaseData;

	// 현재 페이즈 (1~3)
	UPROPERTY(Replicated)
	int32 CurrentPhase = 1;

	// 페이즈 전환 중 (슈퍼아머 상태)
	UPROPERTY(Replicated)
	bool bIsTransitioning = false;

	// 패턴 실행 중 슈퍼아머 (BTTask_BossAction에서 제어)
	UPROPERTY(Replicated)
	bool bHasSuperArmor = false;

	// 순찰 활성화 토글 (인게임에서 코드/BP로 제어)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Boss|Patrol")
	bool bPatrolEnabled = true;

	// 순찰 이동 속도 (전투 속도보다 느리게)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Patrol")
	float PatrolWalkSpeed = 200.0f;

	// 순간이동 이펙트 (사라지기) - Niagara 우선
	UPROPERTY(EditDefaultsOnly, Category = "Boss|Teleport")
	TObjectPtr<UNiagaraSystem> TeleportDisappearEffect;

	// 순간이동 이펙트 (사라지기) - Cascade 폴백
	UPROPERTY(EditDefaultsOnly, Category = "Boss|Teleport")
	TObjectPtr<UParticleSystem> TeleportDisappearEffect_Cascade;

	// 순간이동 이펙트 (나타나기) - Niagara 우선
	UPROPERTY(EditDefaultsOnly, Category = "Boss|Teleport")
	TObjectPtr<UNiagaraSystem> TeleportAppearEffect;

	// 순간이동 이펙트 (나타나기) - Cascade 폴백
	UPROPERTY(EditDefaultsOnly, Category = "Boss|Teleport")
	TObjectPtr<UParticleSystem> TeleportAppearEffect_Cascade;

	// 순간이동 사운드
	UPROPERTY(EditDefaultsOnly, Category = "Boss|Teleport")
	TObjectPtr<USoundBase> TeleportSound;

private:
	// 기본 이동 속도 (페이즈 배율 적용 기준)
	float BaseMaxWalkSpeed = 0.0f;
};
