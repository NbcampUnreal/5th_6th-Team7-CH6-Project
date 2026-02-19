// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_MonsterAttack.generated.h"

// 근접 단일 공격 (몽타주 재생 후 완료)
UCLASS()
class NECROMANCER_API UBTTask_MonsterAttack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_MonsterAttack();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

	UPROPERTY(EditAnywhere,Category="Attack")
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditAnywhere, Category="Attack")
	float TimeoutBuffer = 2.0f;

private:
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted, UBehaviorTreeComponent* OwnerComp);
	void OnSafetyTimeout(UBehaviorTreeComponent* OwnerComp);
	void CleanupAttackState(UBehaviorTreeComponent* OwnerComp);

	FTimerHandle SafetyTimerHandle;
	bool bTaskActive = false;
};
