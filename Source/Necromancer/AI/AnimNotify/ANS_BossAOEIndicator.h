#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANS_BossAOEIndicator.generated.h"

class UDecalComponent;
class UMaterialInstanceDynamic;

// 보스 AOE 범위 표시 (데미지 전 경고 원, 보스로부터 점점 커짐)
// 몽타주 타임라인에서 AN_BossAOEDamage 앞에 배치하여 데미지 타이밍과 분리
//
// 사용법:
// 1. 원형 Deferred Decal 머티리얼 생성 (파라미터: Color, Progress, Opacity)
// 2. 몽타주에 이 노티파이 스테이트 추가, 길이로 확장 시간 조절
// 3. AN_BossAOEDamage를 이 노티파이 끝부분 또는 직후에 배치
UCLASS()
class NECROMANCER_API UANS_BossAOEIndicator : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
	// AOE 중심 소켓 (비어있으면 ActorLocation 사용)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Indicator")
	FName AOESocketName;

	// 최종 표시 반경 (AN_BossAOEDamage의 AOERadius와 동일하게 설정 권장)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Indicator")
	float IndicatorRadius = 400.0f;

	// 데칼 머티리얼 (원형 경고 표시, Deferred Decal 도메인)
	// 권장 파라미터: Color(Vector), Progress(Scalar 0~1), Opacity(Scalar)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Indicator")
	UMaterialInterface* IndicatorMaterial = nullptr;

	// 표시 색상 (머티리얼의 "Color" 파라미터로 전달)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Indicator")
	FLinearColor IndicatorColor = FLinearColor(1.0f, 0.0f, 0.0f, 0.7f);

	// 데칼 투영 깊이 (얇을수록 바닥면에만 투영, 두꺼우면 주변 오브젝트도 물듦)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Indicator")
	float DecalProjectionDepth = 50.0f;

private:
	// 활성 인디케이터 데이터
	struct FActiveIndicator
	{
		TWeakObjectPtr<UDecalComponent> Decal;
		UMaterialInstanceDynamic* DynamicMaterial = nullptr;
		float StartWorldTime = 0.0f;   // NotifyBegin 시점의 월드 시간 (절대 시간 기준)
		float RealDuration = 0.0f;     // 실제 재생 시간 (PlayRate 반영)
		FVector CenterLocation = FVector::ZeroVector;
	};

	// MeshComp별 인디케이터 추적 (멀티플레이어 지원)
	TMap<TWeakObjectPtr<USkeletalMeshComponent>, FActiveIndicator> ActiveIndicators;

	void CleanupIndicator(USkeletalMeshComponent* MeshComp);
};
