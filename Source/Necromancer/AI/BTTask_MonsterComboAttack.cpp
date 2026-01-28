// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_MonsterComboAttack.h"
#include "ComboAttackData.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_MonsterComboAttack::UBTTask_MonsterComboAttack()
{
    NodeName = "Monster Combo Attack";
    bCreateNodeInstance = true;  // 인스턴스별 상태 유지
}

EBTNodeResult::Type UBTTask_MonsterComboAttack::ExecuteTask(
    UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    // 유효성 검사
    if (!ComboData || ComboData->ComboSequence.Num() == 0)
    {
        return EBTNodeResult::Failed;
    }

    AAIController* AIC = OwnerComp.GetAIOwner();
    if (!AIC) return EBTNodeResult::Failed;

    ACharacter* Character = Cast<ACharacter>(AIC->GetPawn());
    if (!Character) return EBTNodeResult::Failed;

    UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
    if (!AnimInstance) return EBTNodeResult::Failed;

    // 콤보 리셋 체크 (시간 초과 시 1타로)
    float CurrentTime = GetWorld()->GetTimeSeconds();
    if (CurrentTime - LastAttackTime > ComboData->ComboResetTime)
    {
        CurrentComboIndex = 0;
    }

    // 현재 콤보 데이터 가져오기
    const FComboAttackData* CurrentAttack = ComboData->GetComboData(CurrentComboIndex);
    if (!CurrentAttack || !CurrentAttack->AttackMontage)
    {
        return EBTNodeResult::Failed;
    }

    // 몽타주 재생
    float Duration = Character->PlayAnimMontage(CurrentAttack->AttackMontage);
    if (Duration <= 0.0f)
    {
        return EBTNodeResult::Failed;
    }

    // 상태 업데이트
    LastAttackTime = CurrentTime;
    CachedOwnerComp = &OwnerComp;

    // Blackboard 업데이트 (옵션)
    if (UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent())
    {
        BB->SetValueAsBool(FName("IsAttacking"), true);
        BB->SetValueAsInt(FName("CurrentComboIndex"), CurrentComboIndex);
    }

    // 몽타주 종료 델리게이트
    FOnMontageEnded EndDelegate;
    EndDelegate.BindUObject(this, &UBTTask_MonsterComboAttack::OnMontageEnded);
    AnimInstance->Montage_SetEndDelegate(EndDelegate, CurrentAttack->AttackMontage);

    // 다음 콤보 인덱스 준비
    CurrentComboIndex = (CurrentComboIndex + 1) % ComboData->GetMaxComboCount();

    return EBTNodeResult::InProgress;
}

void UBTTask_MonsterComboAttack::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    if (!CachedOwnerComp) return;

    if (UBlackboardComponent* BB = CachedOwnerComp->GetBlackboardComponent())
    {
        BB->SetValueAsBool(FName("IsAttacking"), false);
    }

    FinishLatentTask(*CachedOwnerComp, 
        bInterrupted ? EBTNodeResult::Failed : EBTNodeResult::Succeeded);
}

void UBTTask_MonsterComboAttack::ResetCombo()
{
    CurrentComboIndex = 0;
}