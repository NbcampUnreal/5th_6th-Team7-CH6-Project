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

	// 에디터에서 콤보 데이터 에셋 할당
	UPROPERTY(EditAnywhere, Category = "Combo")
	UMonsterComboDataAsset* ComboData;

private:
	int32 CurrentComboIndex = 0;

	UPROPERTY()
	UBehaviorTreeComponent* CachedOwnerComp;

	UPROPERTY()
	ACharacter* CachedCharacter;

	// 콤보 전환 중인지 (interrupt를 콤보 vs 외부로 구분)
	bool bComboTransitioning = false;

	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	// 다음 콤보 몽타주 재생
	void PlayNextCombo();
	// 콤보 연결 처리 (거리/조건 체크)
	void HandleNextCombo();
	// 콤보 종료 및 BT 결과 반환
	void FinishCombo(EBTNodeResult::Type Result);
};
