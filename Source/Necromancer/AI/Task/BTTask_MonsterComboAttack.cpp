// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_MonsterComboAttack.h"
#include "ComboAttackData.h"
#include "AIController.h"
#include "MonsterBase.h"
#include "MonsterStatComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_MonsterComboAttack::UBTTask_MonsterComboAttack()
{
    NodeName = "Monster Combo Attack";
    bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTTask_MonsterComboAttack::ExecuteTask(
    UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    if (!ComboData || ComboData->ComboSequence.Num() == 0)
    {
        return EBTNodeResult::Failed;
    }

    AAIController* AIC = OwnerComp.GetAIOwner();
    if (!AIC)
    {
        return EBTNodeResult::Failed;
    }

    ACharacter* Character = Cast<ACharacter>(AIC->GetPawn());
    if (!Character)
    {
        return EBTNodeResult::Failed;
    }

    // 쿨타임 체크
    UMonsterStatComponent* StatComp = Character->FindComponentByClass<UMonsterStatComponent>();
    if (StatComp && !StatComp->CanAttack())
    {
        return EBTNodeResult::Failed;
    }

    // 공격 중 이동 정지
    Character->GetCharacterMovement()->StopMovementImmediately();
    Character->GetCharacterMovement()->DisableMovement();

    CurrentComboIndex = 0;
    bComboTransitioning = false;
    CachedOwnerComp = &OwnerComp;
    CachedCharacter = Character;

    if (UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent())
    {
        BB->SetValueAsBool(FName("IsAttacking"), true);
    }

    // 델리게이트 바인딩 — 노티파이가 호출하면 HandleNextCombo 즉시 실행
    if (AMonsterBase* Monster = Cast<AMonsterBase>(Character))
    {
        Monster->OnNextComboRequested.BindUObject(this, &UBTTask_MonsterComboAttack::HandleNextCombo);
    }

    PlayNextCombo();

    return EBTNodeResult::InProgress;
}

void UBTTask_MonsterComboAttack::HandleNextCombo()
{
    if (!CachedCharacter || !ComboData)
    {
        return;
    }

    CurrentComboIndex++;
    if (CurrentComboIndex >= ComboData->GetMaxComboCount())
    {
        return;
    }

    // 콤보 전환 중 표시 — OnMontageEnded에서 해제
    bComboTransitioning = true;

    PlayNextCombo();
}

void UBTTask_MonsterComboAttack::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    if (!CachedOwnerComp)
    {
        return;
    }

    // 콤보 전환으로 인한 interrupt → 무시 (HandleNextCombo가 이미 다음 몽타주를 재생함)
    if (bInterrupted && bComboTransitioning)
    {
        bComboTransitioning = false;
        return;
    }

    // 외부 중단 (Stagger, Stun 등)
    if (bInterrupted)
    {
        FinishCombo(EBTNodeResult::Failed);
        return;
    }

    // 자연 종료 — 콤보 끝
    FinishCombo(EBTNodeResult::Succeeded);
}

void UBTTask_MonsterComboAttack::FinishCombo(EBTNodeResult::Type Result)
{
    // 델리게이트 언바인드
    if (AMonsterBase* Monster = Cast<AMonsterBase>(CachedCharacter))
    {
        Monster->OnNextComboRequested.Unbind();
    }

    if (UBlackboardComponent* BB = CachedOwnerComp->GetBlackboardComponent())
    {
        BB->SetValueAsBool(FName("IsAttacking"), false);
    }

    // 이동 복구 + 쿨타임 마킹
    if (CachedCharacter)
    {
        CachedCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Walking);

        if (UMonsterStatComponent* StatComp = CachedCharacter->FindComponentByClass<UMonsterStatComponent>())
        {
            StatComp->MarkAttackUsed();
        }
    }

    FinishLatentTask(*CachedOwnerComp, Result);
}

void UBTTask_MonsterComboAttack::PlayNextCombo()
{
    if (!CachedCharacter || !ComboData)
    {
        return;
    }

    const FComboAttackData* AttackData = ComboData->GetComboData(CurrentComboIndex);
    if (!AttackData || !AttackData->AttackMontage)
    {
        return;
    }

    UAnimInstance* AnimInstance = CachedCharacter->GetMesh()->GetAnimInstance();
    if (!AnimInstance)
    {
        return;
    }

    AMonsterBase* Monster = Cast<AMonsterBase>(CachedCharacter);
    if (Monster)
    {
        Monster->Multicast_PlayMontage(AttackData->AttackMontage);
    }
    
    float Duration = AttackData->AttackMontage->GetPlayLength();
    if (Duration <= 0.0f)
    {
        return;
    }

    FOnMontageEnded EndDelegate;
    EndDelegate.BindUObject(this, &UBTTask_MonsterComboAttack::OnMontageEnded);
    AnimInstance->Montage_SetEndDelegate(EndDelegate, AttackData->AttackMontage);
}
