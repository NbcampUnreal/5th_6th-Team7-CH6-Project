#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "NiagaraSystem.h"
#include "Particles/ParticleSystem.h"
#include "AN_BossAOEDamage.generated.h"

// 보스 AOE 데미지 판정 전용 (원형 범위 내 모든 적에게 데미지)
// 이펙트 연출은 AN_BossAOEEffect에서 담당, 몽타주 타임라인에서 타이밍 분리 가능
UCLASS()
class NECROMANCER_API UAN_BossAOEDamage : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
	// AOE 중심 소켓 (설정 시 소켓 위치 기준 판정, 비어있으면 ActorLocation)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AOE")
	FName AOESocketName;

	// AOE 반경
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AOE")
	float AOERadius = 400.0f;

	// 히트 이펙트 (Niagara 우선)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hit Effect")
	UNiagaraSystem* HitParticle = nullptr;

	// 히트 이펙트 (Cascade 폴백 - Niagara가 없을 때 사용)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hit Effect")
	UParticleSystem* HitParticle_Cascade = nullptr;

	// 히트 파티클 스케일
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hit Effect")
	FVector HitParticleScale = FVector(1.0f);

	// 히트 사운드
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hit Effect")
	USoundBase* HitSound = nullptr;
};
