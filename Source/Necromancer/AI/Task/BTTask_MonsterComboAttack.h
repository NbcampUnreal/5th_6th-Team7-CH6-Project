// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_MonsterComboAttack.generated.h"

class UMonsterComboDataAsset;
// 콤보 공격 (DataAsset 기반 연속 몽타주)
UCLASS()
class NECROMANCER_API UBTTask_MonsterComboAttack : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTask_MonsterComboAttack();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

	UPROPERTY(EditAnywhere, Category = "Combo")
	UMonsterComboDataAsset* ComboData;

	UPROPERTY(EditAnywhere, Category = "Combo")
	float TimeoutBufferPerHit = 1.5f;

private:
	int32 CurrentComboIndex = 0;

	UPROPERTY()
	UBehaviorTreeComponent* CachedOwnerComp;

	UPROPERTY()
	ACharacter* CachedCharacter;

	bool bComboTransitioning = false;
	bool bTaskActive = false;
	FTimerHandle SafetyTimerHandle;

	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	void PlayNextCombo();
	void HandleNextCombo();
	void FinishCombo(EBTNodeResult::Type Result);
	void OnSafetyTimeout();
};
