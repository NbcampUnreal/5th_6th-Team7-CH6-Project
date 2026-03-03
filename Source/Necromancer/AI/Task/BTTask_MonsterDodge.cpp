#include "BTTask_MonsterDodge.h"
#include "AIController.h"
#include "MonsterBase.h"
#include "MonsterStatComponent.h"
#include "Necromancer.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"

UBTTask_MonsterDodge::UBTTask_MonsterDodge()
{
	NodeName = "Monster Dodge";
	bCreateNodeInstance = true;
	bNotifyTick = false;
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UBTTask_MonsterDodge::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIC = OwnerComp.GetAIOwner();
	if (!AIC)
	{
		return EBTNodeResult::Failed;
	}

	AMonsterBase* Monster = Cast<AMonsterBase>(AIC->GetPawn());
	if (!Monster)
	{
		return EBTNodeResult::Failed;
	}

	// SkillLevel 기반 확률 판정
	UMonsterStatComponent* StatComp = Monster->FindComponentByClass<UMonsterStatComponent>();
	if (StatComp)
	{
		float FinalChance = BaseDodgeChance * StatComp->GetSkillLevel();
		if (FMath::FRand() > FinalChance)
		{
			return EBTNodeResult::Failed;
		}
	}

	UAnimMontage* SelectedMontage = SelectDodgeMontage();
	if (!SelectedMontage)
	{
		return EBTNodeResult::Failed;
	}

	USkeletalMeshComponent* Mesh = Monster->GetMesh();
	if (!Mesh)
	{
		return EBTNodeResult::Failed;
	}

	UAnimInstance* AnimInstance = Mesh->GetAnimInstance();
	if (!AnimInstance)
	{
		return EBTNodeResult::Failed;
	}

	Monster->GetCharacterMovement()->StopMovementImmediately();
	Monster->Multicast_PlayMontage(SelectedMontage);

	float Duration = SelectedMontage->GetPlayLength();
	if (Duration <= 0.0f)
	{
		return EBTNodeResult::Failed;
	}

	bTaskActive = true;

	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &UBTTask_MonsterDodge::OnMontageEnded, &OwnerComp);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, SelectedMontage);

	if (UWorld* World = Monster->GetWorld())
	{
		World->GetTimerManager().SetTimer(
			SafetyTimerHandle,
			FTimerDelegate::CreateUObject(this, &UBTTask_MonsterDodge::OnSafetyTimeout, &OwnerComp),
			Duration + TimeoutBuffer,
			false
		);
	}

	return EBTNodeResult::InProgress;
}

UAnimMontage* UBTTask_MonsterDodge::SelectDodgeMontage() const
{
	TArray<UAnimMontage*> Available;
	if (DodgeBackMontage)
	{
		Available.Add(DodgeBackMontage);
	}
	if (DodgeLeftMontage)
	{
		Available.Add(DodgeLeftMontage);
	}
	if (DodgeRightMontage)
	{
		Available.Add(DodgeRightMontage);
	}

	if (Available.Num() == 0)
	{
		return nullptr;
	}

	return Available[FMath::RandRange(0, Available.Num() - 1)];
}

void UBTTask_MonsterDodge::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted, UBehaviorTreeComponent* OwnerComp)
{
	if (!OwnerComp || !bTaskActive)
	{
		return;
	}

	CleanupState(OwnerComp);
	FinishLatentTask(*OwnerComp, bInterrupted ? EBTNodeResult::Failed : EBTNodeResult::Succeeded);
}

void UBTTask_MonsterDodge::OnSafetyTimeout(UBehaviorTreeComponent* OwnerComp)
{
	if (!OwnerComp || !bTaskActive)
	{
		return;
	}

	CleanupState(OwnerComp);
	FinishLatentTask(*OwnerComp, EBTNodeResult::Failed);
}

void UBTTask_MonsterDodge::CleanupState(UBehaviorTreeComponent* OwnerComp)
{
	bTaskActive = false;

	if (AAIController* AIC = OwnerComp->GetAIOwner())
	{
		if (APawn* Pawn = AIC->GetPawn())
		{
			Pawn->GetWorld()->GetTimerManager().ClearTimer(SafetyTimerHandle);

			if (AMonsterBase* Monster = Cast<AMonsterBase>(Pawn))
			{
				Monster->RestoreMovementIfAlive();
			}
		}
	}
}

void UBTTask_MonsterDodge::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	if (bTaskActive)
	{
		CleanupState(&OwnerComp);
	}

	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}
