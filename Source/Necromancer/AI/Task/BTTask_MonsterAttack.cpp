// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_MonsterAttack.h"
#include "AIController.h"
#include "MonsterAttackSetData.h"
#include "MonsterBase.h"
#include "MonsterEngagementSubsystem.h"
#include "MonsterStatComponent.h"
#include "Necromancer.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "TimerManager.h"

UBTTask_MonsterAttack::UBTTask_MonsterAttack()
{
	NodeName = "Monster Attack";
	bCreateNodeInstance = true;
	bNotifyTick = false;
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UBTTask_MonsterAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
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

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB)
	{
		return EBTNodeResult::Failed;
	}

	USkeletalMeshComponent* Mesh = Character->GetMesh();
	if (!Mesh)
	{
		return EBTNodeResult::Failed;
	}

	UAnimInstance* AnimInstance = Mesh->GetAnimInstance();
	if (!AnimInstance)
	{
		return EBTNodeResult::Failed;
	}

	// 공격 몽타주 결정: AttackSet이 있으면 거리 기반 랜덤, 없으면 단일 몽타주
	UAnimMontage* SelectedMontage = AttackMontage;
	float DamageMultiplier = 1.0f;

	if (AttackSet)
	{
		AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(NAME_TargetActor));
		if (TargetActor)
		{
			float Distance = FVector::Dist(Character->GetActorLocation(), TargetActor->GetActorLocation());
			const FMonsterAttackEntry* SelectedEntry = AttackSet->SelectAttackByDistance(Distance);
			if (SelectedEntry)
			{
				SelectedMontage = SelectedEntry->AttackMontage;
				DamageMultiplier = SelectedEntry->DamageMultiplier;
			}
		}
	}

	// 데미지 배율 적용
	if (StatComp)
	{
		StatComp->SetDamageMultiplier(DamageMultiplier);
	}

	// AI 이동 명령 중지 (루트모션은 유지)
	AIC->StopMovement();
	Character->GetCharacterMovement()->StopMovementImmediately();

	AMonsterBase* Monster = Cast<AMonsterBase>(Character);

	// 워프 타겟 설정 (타겟이 있을 때만, 루트모션+AnimNotify로 자연스러운 돌진)
	AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(NAME_TargetActor));
	if (Monster && TargetActor)
	{
		FVector TargetLoc = TargetActor->GetActorLocation();
		FRotator LookAtRot = (TargetLoc - Character->GetActorLocation()).Rotation();
		Monster->SetWarpTarget(FName("AttackTarget"), TargetLoc, LookAtRot);
	}

	if (Monster)
	{
		Monster->SetIdleSoundActive(false);
		Monster->Multicast_PlayMontage(SelectedMontage);
	}

	float Duration = SelectedMontage ? SelectedMontage->GetPlayLength() : 0.0f;
	if (Duration <= 0.0f)
	{
		Character->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		BB->SetValueAsBool(NAME_IsAttacking, false);
		return EBTNodeResult::Failed;
	}
	BB->SetValueAsBool(NAME_IsAttacking, true);
	bTaskActive = true;

	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this,&UBTTask_MonsterAttack::OnMontageEnded,&OwnerComp);
	AnimInstance->Montage_SetEndDelegate(EndDelegate,SelectedMontage);

	if (UWorld* World = Character->GetWorld())
	{
		World->GetTimerManager().SetTimer(
			SafetyTimerHandle,
			FTimerDelegate::CreateUObject(this, &UBTTask_MonsterAttack::OnSafetyTimeout, &OwnerComp),
			Duration + TimeoutBuffer,
			false
		);
	}

	return EBTNodeResult::InProgress;
}

void UBTTask_MonsterAttack::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted, UBehaviorTreeComponent* OwnerComp)
{
	if (!OwnerComp || !bTaskActive)
	{
		return;
	}

	CleanupAttackState(OwnerComp);
	FinishLatentTask(*OwnerComp, bInterrupted ? EBTNodeResult::Failed : EBTNodeResult::Succeeded);
}

void UBTTask_MonsterAttack::OnSafetyTimeout(UBehaviorTreeComponent* OwnerComp)
{
	if (!OwnerComp || !bTaskActive)
	{
		return;
	}

	UE_LOG(LogMonsterAI, Warning, TEXT("[BTTask_MonsterAttack] Safety timeout - forcing cleanup"));
	CleanupAttackState(OwnerComp);
	FinishLatentTask(*OwnerComp, EBTNodeResult::Failed);
}

void UBTTask_MonsterAttack::CleanupAttackState(UBehaviorTreeComponent* OwnerComp)
{
	bTaskActive = false;

	// Safety timer 정리 (어디서 호출되든 항상 정리)
	if (AAIController* TimerAIC = OwnerComp->GetAIOwner())
	{
		if (APawn* TimerPawn = TimerAIC->GetPawn())
		{
			TimerPawn->GetWorld()->GetTimerManager().ClearTimer(SafetyTimerHandle);
		}
	}

	UBlackboardComponent* BB = OwnerComp->GetBlackboardComponent();
	if (BB)
	{
		BB->SetValueAsBool(NAME_IsAttacking, false);
	}

	if (AAIController* AIC = OwnerComp->GetAIOwner())
	{
		if (APawn* Pawn = AIC->GetPawn())
		{
			if (AMonsterBase* Monster = Cast<AMonsterBase>(Pawn))
			{
				Monster->ClearWarpTarget(FName("AttackTarget"));
				Monster->RestoreMovementIfAlive();
				Monster->SetIdleSoundActive(true);
			}
			else if (ACharacter* Char = Cast<ACharacter>(Pawn))
			{
				Char->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
			}

			if (UMonsterStatComponent* StatComp = Pawn->FindComponentByClass<UMonsterStatComponent>())
			{
				StatComp->MarkAttackUsed();
				StatComp->ResetDamageMultiplier();
			}

			if (BB)
			{
				if (UWorld* World = Pawn->GetWorld())
				{
					if (UMonsterEngagementSubsystem* Engagement = World->GetSubsystem<UMonsterEngagementSubsystem>())
					{
						AActor* Target = Cast<AActor>(BB->GetValueAsObject(NAME_TargetActor));
						if (Target)
						{
							Engagement->RefreshSlotAfterAttack(Pawn, Target);
						}
					}
				}
			}
		}
	}
}

void UBTTask_MonsterAttack::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	if (bTaskActive)
	{
		CleanupAttackState(&OwnerComp);
	}

	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}
