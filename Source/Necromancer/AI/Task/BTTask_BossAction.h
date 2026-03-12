#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BossActionDataAsset.h"
#include "BTTask_BossAction.generated.h"

class ABossMonsterBase;

// 보스 통합 액션 Task (Melee/AOE/Teleport을 ActionType에 따라 분기 실행)
UCLASS()
class NECROMANCER_API UBTTask_BossAction : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_BossAction();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

	// 보스 액션 세트
	UPROPERTY(EditAnywhere, Category = "Boss")
	TObjectPtr<UBossActionDataAsset> ActionSet;

	// 안전 타이머 추가 시간
	UPROPERTY(EditAnywhere, Category = "Attack")
	float TimeoutBuffer = 2.0f;

private:
	// 타입별 실행
	void ExecuteMeleeAction(ABossMonsterBase* Boss, const FBossActionEntry& Action, UBehaviorTreeComponent& OwnerComp);
	void ExecuteAOEAction(ABossMonsterBase* Boss, const FBossActionEntry& Action, UBehaviorTreeComponent& OwnerComp);
	void ExecuteTeleportAction(ABossMonsterBase* Boss, const FBossActionEntry& Action, UBehaviorTreeComponent& OwnerComp);

	// 순간이동 위치 계산
	FVector CalculateTeleportLocation(AActor* Target, const FBossActionEntry& Action) const;

	// 몽타주 재생 공통 처리
	void PlayActionMontage(ABossMonsterBase* Boss, UAnimMontage* Montage, UBehaviorTreeComponent& OwnerComp);

	// 콜백
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted, UBehaviorTreeComponent* OwnerComp);
	void OnSafetyTimeout(UBehaviorTreeComponent* OwnerComp);
	void CleanupAttackState(UBehaviorTreeComponent* OwnerComp);

	// 개별 쿨타임 추적 (액션 인덱스 → 마지막 사용 월드 시간)
	TMap<int32, float> ActionCooldowns;

	FTimerHandle SafetyTimerHandle;
	bool bTaskActive = false;
};
