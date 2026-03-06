#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_MonsterDodge.generated.h"

// 방향별 회피 (몽타주 재생 후 완료)
UCLASS()
class NECROMANCER_API UBTTask_MonsterDodge : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_MonsterDodge();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

	// 후방 회피
	UPROPERTY(EditAnywhere, Category = "Dodge")
	TObjectPtr<UAnimMontage> DodgeBackMontage;

	// 좌측 회피
	UPROPERTY(EditAnywhere, Category = "Dodge")
	TObjectPtr<UAnimMontage> DodgeLeftMontage;

	// 우측 회피
	UPROPERTY(EditAnywhere, Category = "Dodge")
	TObjectPtr<UAnimMontage> DodgeRightMontage;

	// 회피 기본 확률 (SkillLevel과 곱해짐)
	UPROPERTY(EditAnywhere, Category = "Dodge", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float BaseDodgeChance = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Dodge")
	float TimeoutBuffer = 1.0f;

private:
	UAnimMontage* SelectDodgeMontage() const;
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted, UBehaviorTreeComponent* OwnerComp);
	void OnSafetyTimeout(UBehaviorTreeComponent* OwnerComp);
	void CleanupState(UBehaviorTreeComponent* OwnerComp);

	FTimerHandle SafetyTimerHandle;
	bool bTaskActive = false;
};
