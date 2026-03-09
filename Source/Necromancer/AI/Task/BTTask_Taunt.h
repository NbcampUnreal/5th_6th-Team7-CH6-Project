#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_Taunt.generated.h"

class AMonsterBase;

// 타운트: 몽타주 재생 + 주변 몬스터에게 직접 타겟 전파 (한 번만 실행)
UCLASS()
class NECROMANCER_API UBTTask_Taunt : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_Taunt();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;
	virtual FString GetStaticDescription() const override;

	// 타운트 몽타주
	UPROPERTY(EditAnywhere, Category = "Taunt")
	TObjectPtr<UAnimMontage> TauntMontage;

	// 어그로 전파 범위
	UPROPERTY(EditAnywhere, Category = "Taunt")
	float AggroRange = 1500.0f;

	// 몽타주 안전 타이머 여유분
	UPROPERTY(EditAnywhere, Category = "Taunt")
	float TimeoutBuffer = 1.0f;

private:
	// 범위 내 몬스터에게 직접 타겟 전파
	void PropagateAggroToNearby(AMonsterBase* SourceMonster, AActor* Target);

	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted, UBehaviorTreeComponent* OwnerComp);
	void OnSafetyTimeout(UBehaviorTreeComponent* OwnerComp);
	void CleanupState(UBehaviorTreeComponent* OwnerComp);

	FTimerHandle SafetyTimerHandle;
	bool bTaskActive = false;
};
