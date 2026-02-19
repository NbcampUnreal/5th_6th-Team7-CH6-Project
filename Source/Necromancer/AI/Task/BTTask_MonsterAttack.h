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
	
	UPROPERTY(EditAnywhere,Category="Attack")
	TObjectPtr<UAnimMontage> AttackMontage;
	
private:
	// 몽타주 종료 콜백
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted, UBehaviorTreeComponent* OwnerComp);
};
