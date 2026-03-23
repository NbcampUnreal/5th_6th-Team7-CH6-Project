// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_MonsterComboAttack.h"
#include "ComboAttackData.h"
#include "AIController.h"
#include "MonsterBase.h"
#include "MonsterEngagementSubsystem.h"
#include "MonsterStatComponent.h"
#include "Necromancer.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "TimerManager.h"

UBTTask_MonsterComboAttack::UBTTask_MonsterComboAttack()
{
    NodeName = "Monster Combo Attack";
    bCreateNodeInstance = true;
    bNotifyTaskFinished = true;
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

    UMonsterStatComponent* StatComp = Character->FindComponentByClass<UMonsterStatComponent>();
    if (StatComp && !StatComp->CanAttack())
    {
        return EBTNodeResult::Failed;
    }

    Character->GetCharacterMovement()->StopMovementImmediately();
    Character->GetCharacterMovement()->DisableMovement();

    CurrentComboIndex = 0;
    bComboTransitioning = false;
    bTaskActive = true;
    CachedOwnerComp = &OwnerComp;
    CachedCharacter = Character;

    if (UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent())
    {
        BB->SetValueAsBool(NAME_IsAttacking, true);
    }

    if (AMonsterBase* Monster = Cast<AMonsterBase>(Character))
    {
        Monster->OnNextComboRequested.BindUObject(this, &UBTTask_MonsterComboAttack::HandleNextCombo);
    }

    PlayNextCombo();

    // 전체 콤보 시간 + 버퍼로 타임아웃 설정
    float TotalDuration = 0.0f;
    int32 MaxCombo = ComboData->GetMaxComboCount();
    for (int32 i = 0; i < MaxCombo; ++i)
    {
        if (const FComboAttackData* Data = ComboData->GetComboData(i))
        {
            if (Data->AttackMontage)
            {
                TotalDuration += Data->AttackMontage->GetPlayLength();
            }
        }
    }
    TotalDuration += TimeoutBufferPerHit * MaxCombo;

    if (UWorld* World = Character->GetWorld())
    {
        World->GetTimerManager().SetTimer(
            SafetyTimerHandle,
            FTimerDelegate::CreateUObject(this, &UBTTask_MonsterComboAttack::OnSafetyTimeout),
            TotalDuration,
            false
        );
    }

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

    bComboTransitioning = true;
    PlayNextCombo();
}

void UBTTask_MonsterComboAttack::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    if (!CachedOwnerComp || !bTaskActive)
    {
        return;
    }

    if (bInterrupted && bComboTransitioning)
    {
        bComboTransitioning = false;
        return;
    }

    if (bInterrupted)
    {
        FinishCombo(EBTNodeResult::Failed);
        return;
    }

    FinishCombo(EBTNodeResult::Succeeded);
}

void UBTTask_MonsterComboAttack::FinishCombo(EBTNodeResult::Type Result)
{
    if (!bTaskActive)
    {
        return;
    }
    bTaskActive = false;

    if (CachedCharacter)
    {
        if (UWorld* World = CachedCharacter->GetWorld())
        {
            World->GetTimerManager().ClearTimer(SafetyTimerHandle);
        }
    }

    if (AMonsterBase* Monster = Cast<AMonsterBase>(CachedCharacter))
    {
        Monster->OnNextComboRequested.Unbind();
        Monster->ClearWarpTarget(FName("AttackTarget"));
    }

    if (CachedOwnerComp)
    {
        if (UBlackboardComponent* BB = CachedOwnerComp->GetBlackboardComponent())
        {
            BB->SetValueAsBool(NAME_IsAttacking, false);
        }
    }

    if (CachedCharacter)
    {
        if (AMonsterBase* Monster = Cast<AMonsterBase>(CachedCharacter))
        {
            Monster->RestoreMovementIfAlive();
        }
        else
        {
            CachedCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
        }

        if (UMonsterStatComponent* StatComp = CachedCharacter->FindComponentByClass<UMonsterStatComponent>())
        {
            StatComp->MarkAttackUsed();
        }

        // 공격 완료 후 슬롯 유지
        if (CachedOwnerComp)
        {
            if (UBlackboardComponent* BB = CachedOwnerComp->GetBlackboardComponent())
            {
                if (UWorld* World = CachedCharacter->GetWorld())
                {
                    if (UMonsterEngagementSubsystem* Engagement = World->GetSubsystem<UMonsterEngagementSubsystem>())
                    {
                        AActor* Target = Cast<AActor>(BB->GetValueAsObject(NAME_TargetActor));
                        if (Target)
                        {
                            Engagement->RefreshSlotAfterAttack(CachedCharacter, Target);
                        }
                    }
                }
            }
        }
    }

    if (CachedOwnerComp)
    {
        FinishLatentTask(*CachedOwnerComp, Result);
    }
}

void UBTTask_MonsterComboAttack::PlayNextCombo()
{
    if (!CachedCharacter || !ComboData)
    {
        FinishCombo(EBTNodeResult::Failed);
        return;
    }

    const FComboAttackData* AttackData = ComboData->GetComboData(CurrentComboIndex);
    if (!AttackData || !AttackData->AttackMontage)
    {
        FinishCombo(EBTNodeResult::Failed);
        return;
    }

    USkeletalMeshComponent* Mesh = CachedCharacter->GetMesh();
    if (!Mesh)
    {
        FinishCombo(EBTNodeResult::Failed);
        return;
    }

    UAnimInstance* AnimInstance = Mesh->GetAnimInstance();
    if (!AnimInstance)
    {
        FinishCombo(EBTNodeResult::Failed);
        return;
    }

    AMonsterBase* Monster = Cast<AMonsterBase>(CachedCharacter);

    // 콤보 각 타격마다 워프 타겟 업데이트
    if (Monster && CachedOwnerComp)
    {
        if (UBlackboardComponent* BB = CachedOwnerComp->GetBlackboardComponent())
        {
            AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(NAME_TargetActor));
            if (TargetActor)
            {
                FVector TargetLoc = TargetActor->GetActorLocation();
                FRotator LookAtRot = (TargetLoc - CachedCharacter->GetActorLocation()).Rotation();
                Monster->SetWarpTarget(FName("AttackTarget"), TargetLoc, LookAtRot);
            }
        }
    }

    if (Monster)
    {
        Monster->Multicast_PlayMontage(AttackData->AttackMontage);
    }

    float Duration = AttackData->AttackMontage->GetPlayLength();
    if (Duration <= 0.0f)
    {
        FinishCombo(EBTNodeResult::Failed);
        return;
    }

    FOnMontageEnded EndDelegate;
    EndDelegate.BindUObject(this, &UBTTask_MonsterComboAttack::OnMontageEnded);
    AnimInstance->Montage_SetEndDelegate(EndDelegate, AttackData->AttackMontage);
}

void UBTTask_MonsterComboAttack::OnSafetyTimeout()
{
    UE_LOG(LogTemp, Warning, TEXT("[BTTask_MonsterComboAttack] Safety timeout - forcing cleanup"));
    FinishCombo(EBTNodeResult::Failed);
}

void UBTTask_MonsterComboAttack::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
    if (bTaskActive)
    {
        FinishCombo(EBTNodeResult::Failed);
    }

    Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}
