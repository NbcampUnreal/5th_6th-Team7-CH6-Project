#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BossPhaseDataAsset.generated.h"

USTRUCT(BlueprintType)
struct FBossPhaseConfig
{
	GENERATED_BODY()

	// 이 페이즈 진입 HP 비율 (Phase1=1.0, Phase2=0.7, Phase3=0.3)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float HealthThreshold = 1.0f;

	// 공격력 배율
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.1"))
	float AttackPowerMultiplier = 1.0f;

	// 이동 속도 배율
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.1"))
	float MovementSpeedMultiplier = 1.0f;

	// 페이즈 전환 시 재생할 몽타주 (없으면 전환 연출 없음)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> PhaseTransitionMontage = nullptr;
};

UCLASS(BlueprintType)
class NECROMANCER_API UBossPhaseDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	// 페이즈 설정 (인덱스 0=Phase1, 1=Phase2, 2=Phase3)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Phases")
	TArray<FBossPhaseConfig> Phases;

	const FBossPhaseConfig* GetPhaseConfig(int32 PhaseIndex) const
	{
		// PhaseIndex는 1-based (Phase1=1, Phase2=2, Phase3=3)
		int32 ArrayIndex = PhaseIndex - 1;
		if (Phases.IsValidIndex(ArrayIndex))
		{
			return &Phases[ArrayIndex];
		}
		return nullptr;
	}
};
