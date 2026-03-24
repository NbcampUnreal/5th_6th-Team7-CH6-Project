// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_RangedAttack.h"
#include "AIController.h"
#include "MonsterBase.h"
#include "MonsterEngagementSubsystem.h"
#include "MonsterStatComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Necromancer.h"
#include "TimerManager.h"

UBTTask_RangedAttack::UBTTask_RangedAttack()
{
	NodeName = "Ranged Attack";
	bCreateNodeInstance = true;
	bNotifyTick = false;
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UBTTask_RangedAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
	if (!AnimInstance)
	{
		return EBTNodeResult::Failed;
	}

	AMonsterBase* Monster = Cast<AMonsterBase>(Character);
	if (Monster)
	{
		Monster->Multicast_PlayMontage(RangedAttackMontage);
	}

	float Duration = RangedAttackMontage ? RangedAttackMontage->GetPlayLength() : 0.0f;
	if (Duration <= 0.0f)
	{
		Character->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		BB->SetValueAsBool(NAME_IsAttacking, false);
		return EBTNodeResult::Failed;
	}

	BB->SetValueAsBool(NAME_IsAttacking, true);
	bTaskActive = true;

	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &UBTTask_RangedAttack::OnMontageEnded, &OwnerComp);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, RangedAttackMontage);

	if (UWorld* World = Character->GetWorld())
	{
		World->GetTimerManager().SetTimer(
			SafetyTimerHandle,
			FTimerDelegate::CreateUObject(this, &UBTTask_RangedAttack::OnSafetyTimeout, &OwnerComp),
			Duration + TimeoutBuffer,
			false
		);
	}

	return EBTNodeResult::InProgress;
}

void UBTTask_RangedAttack::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted, UBehaviorTreeComponent* OwnerComp)
{
	if (!OwnerComp || !bTaskActive)
	{
		return;
	}

	if (AAIController* AIC = OwnerComp->GetAIOwner())
	{
		if (APawn* Pawn = AIC->GetPawn())
		{
			Pawn->GetWorld()->GetTimerManager().ClearTimer(SafetyTimerHandle);
		}
	}

	CleanupAttackState(OwnerComp);
	FinishLatentTask(*OwnerComp, bInterrupted ? EBTNodeResult::Failed : EBTNodeResult::Succeeded);
}

void UBTTask_RangedAttack::OnSafetyTimeout(UBehaviorTreeComponent* OwnerComp)
{
	if (!OwnerComp || !bTaskActive)
	{
		return;
	}

	UE_LOG(LogMonsterAI, Warning, TEXT("[BTTask_RangedAttack] Safety timeout - forcing cleanup"));
	CleanupAttackState(OwnerComp);
	FinishLatentTask(*OwnerComp, EBTNodeResult::Failed);
}

void UBTTask_RangedAttack::CleanupAttackState(UBehaviorTreeComponent* OwnerComp)
{
	bTaskActive = false;

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
				Monster->RestoreMovementIfAlive();
			}
			else if (ACharacter* Char = Cast<ACharacter>(Pawn))
			{
				Char->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
			}

			if (UMonsterStatComponent* StatComp = Pawn->FindComponentByClass<UMonsterStatComponent>())
			{
				StatComp->MarkAttackUsed();
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
							Engagement->ReleaseAttackSlot(Pawn, Target);
						}
					}
				}
			}
		}
	}
}

void UBTTask_RangedAttack::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	if (bTaskActive)
	{
		if (AAIController* AIC = OwnerComp.GetAIOwner())
		{
			if (APawn* Pawn = AIC->GetPawn())
			{
				Pawn->GetWorld()->GetTimerManager().ClearTimer(SafetyTimerHandle);
			}
		}
		CleanupAttackState(&OwnerComp);
	}

	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}
