#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "NiagaraSystem.h"
#include "Particles/ParticleSystem.h"
#include "AN_BossAOEEffect.generated.h"

// 보스 AOE 이펙트 전용 (데미지 없음, 시각 연출만)
// AN_BossAOEDamage와 분리하여 몽타주 타임라인에서 이펙트/데미지 타이밍을 개별 조절 가능
UCLASS()
class NECROMANCER_API UAN_BossAOEEffect : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
	// AOE 중심 소켓 (설정 시 소켓 위치에서 이펙트 재생, 비어있으면 ActorLocation)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AOE")
	FName AOESocketName;

	// AOE 바닥 이펙트 (Niagara 우선)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	UNiagaraSystem* AOEEffect = nullptr;

	// AOE 바닥 이펙트 (Cascade 폴백 - Niagara가 없을 때 사용)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	UParticleSystem* AOEEffect_Cascade = nullptr;

	// AOE 이펙트 스케일
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	FVector AOEEffectScale = FVector(1.0f);

	// AOE 사운드
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	USoundBase* AOESound = nullptr;
};
