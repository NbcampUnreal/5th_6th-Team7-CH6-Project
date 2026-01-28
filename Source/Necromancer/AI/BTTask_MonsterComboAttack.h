// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_MonsterComboAttack.generated.h"

class UMonsterComboDataAsset;
/**
 * 
 */
UCLASS()
class NECROMANCER_API UBTTask_MonsterComboAttack : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTask_MonsterComboAttack();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	// 에디터에서 콤보 데이터 에셋 할당
	UPROPERTY(EditAnywhere, Category = "Combo")
	UMonsterComboDataAsset* ComboData;

private:
	// 현재 콤보 인덱스
	int32 CurrentComboIndex = 0;

	// 마지막 공격 시간
	float LastAttackTime = 0.0f;

	// 콤보 리셋 타이머
	FTimerHandle ComboResetTimerHandle;

	// 캐시된 참조
	UPROPERTY()
	UBehaviorTreeComponent* CachedOwnerComp;

	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	void ResetCombo();
};
