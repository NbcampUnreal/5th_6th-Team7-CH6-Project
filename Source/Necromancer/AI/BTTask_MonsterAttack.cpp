// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_MonsterAttack.h"
#include "AIController.h"
#include "MonsterBase.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_MonsterAttack::UBTTask_MonsterAttack()
{
	NodeName = "Monster Attack";
	bCreateNodeInstance = true;
	bNotifyTick = false;
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
		Monster->Multicast_PlayMontage(AttackMontage);
	}
	float Duration = AttackMontage ? AttackMontage->GetPlayLength() : 0.0f;
	if (Duration <= 0.0f)
	{
		BB->SetValueAsBool(FName("IsAttacking"), false);
		return EBTNodeResult::Failed;
	}
	BB->SetValueAsBool(FName("IsAttacking"), true);
	
	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this,&UBTTask_MonsterAttack::OnMontageEnded,&OwnerComp);
	AnimInstance->Montage_SetEndDelegate(EndDelegate,AttackMontage);
	
	return EBTNodeResult::InProgress;
	
	
}

void UBTTask_MonsterAttack::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted, UBehaviorTreeComponent* OwnerComp)
{
	if (!OwnerComp)
	{
		return;
	}
	
	UBlackboardComponent* BB = OwnerComp->GetBlackboardComponent();
	if (BB)
	{
		BB->SetValueAsBool(FName("IsAttacking"), false);
	}
	
	FinishLatentTask(*OwnerComp,bInterrupted ? EBTNodeResult::Failed : EBTNodeResult::Succeeded);
	
}
