#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Necromancer.h"
#include "BossActionDataAsset.generated.h"

USTRUCT(BlueprintType)
struct FBossActionEntry
{
	GENERATED_BODY()

	// === 공통 ===

	// 액션 타입
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Common")
	EBossActionType ActionType = EBossActionType::Melee;

	// 액션 몽타주 (Teleport의 경우 이동 후 재생할 몽타주, 없으면 이동만)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Common")
	TObjectPtr<UAnimMontage> ActionMontage = nullptr;

	// 데미지 배율 (기본 공격력에 곱해짐)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Common", meta = (ClampMin = "0.1"))
	float DamageMultiplier = 1.0f;

	// 선택 확률 가중치 (높을수록 선택 확률 증가)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Common", meta = (ClampMin = "0.1"))
	float Weight = 1.0f;

	// 개별 쿨타임 (초, 0이면 쿨타임 없음)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Common", meta = (ClampMin = "0.0"))
	float Cooldown = 0.0f;

	// 이 액션 사용 가능 최소 페이즈
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Common", meta = (ClampMin = "1", ClampMax = "3"))
	int32 MinPhase = 1;

	// 이 액션 사용 가능 최대 페이즈
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Common", meta = (ClampMin = "1", ClampMax = "3"))
	int32 MaxPhase = 3;

	// 슈퍼아머: true면 이 패턴 실행 중 HitReact 무시 (데미지는 정상 적용)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Common")
	bool bSuperArmor = false;

	// === Melee 전용 ===

	// 이 공격이 유효한 최소 거리
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Melee", meta = (ClampMin = "0.0", EditCondition = "ActionType == EBossActionType::Melee", EditConditionHides))
	float MinDistance = 0.0f;

	// 이 공격이 유효한 최대 거리
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Melee", meta = (ClampMin = "0.0", EditCondition = "ActionType == EBossActionType::Melee", EditConditionHides))
	float MaxDistance = 300.0f;

	// === AOE 전용 ===

	// AOE 반경
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AOE", meta = (ClampMin = "0.0", EditCondition = "ActionType == EBossActionType::AOE", EditConditionHides))
	float AOERadius = 400.0f;

	// === Teleport 전용 ===

	// 순간이동 모드
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Teleport", meta = (EditCondition = "ActionType == EBossActionType::Teleport", EditConditionHides))
	ETeleportMode TeleportMode = ETeleportMode::BehindTarget;

	// 타겟으로부터의 순간이동 거리
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Teleport", meta = (ClampMin = "0.0", EditCondition = "ActionType == EBossActionType::Teleport", EditConditionHides))
	float TeleportDistance = 200.0f;
};

UCLASS(BlueprintType)
class NECROMANCER_API UBossActionDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss Actions")
	TArray<FBossActionEntry> Actions;

	// 현재 페이즈+거리+쿨타임 조건에 맞는 액션 중 가중치 랜덤 선택
	// @param CurrentPhase: 현재 보스 페이즈 (1~3)
	// @param DistanceToTarget: 타겟까지 거리 (Melee 필터용)
	// @param CooldownTimers: 액션 인덱스 → 마지막 사용 시간
	// @param CurrentTime: 현재 월드 시간
	// @param OutIndex: 선택된 액션의 인덱스 (쿨타임 추적용)
	const FBossActionEntry* SelectAction(int32 CurrentPhase, float DistanceToTarget,
		const TMap<int32, float>& CooldownTimers, float CurrentTime, int32& OutIndex) const;
};
