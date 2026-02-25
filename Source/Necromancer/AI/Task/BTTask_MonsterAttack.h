// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_MonsterAttack.generated.h"

class UMonsterAttackSetData;

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

	// 단일 공격 몽타주 (AttackSet이 없을 때 사용)
	UPROPERTY(EditAnywhere, Category="Attack")
	TObjectPtr<UAnimMontage> AttackMontage;

	// 거리별 랜덤 공격 셋 (설정하면 AttackMontage 대신 사용)
	UPROPERTY(EditAnywhere, Category="Attack")
	TObjectPtr<UMonsterAttackSetData> AttackSet;

	UPROPERTY(EditAnywhere, Category="Attack")
	float TimeoutBuffer = 2.0f;

private:
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted, UBehaviorTreeComponent* OwnerComp);
	void OnSafetyTimeout(UBehaviorTreeComponent* OwnerComp);
	void CleanupAttackState(UBehaviorTreeComponent* OwnerComp);

	FTimerHandle SafetyTimerHandle;
	bool bTaskActive = false;
};
