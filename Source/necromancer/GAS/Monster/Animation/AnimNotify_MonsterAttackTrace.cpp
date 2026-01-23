// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_MonsterAttackTrace.h"
  #include "GAS/Monster/Interface/MonsterCombatInterface.h"

  UAnimNotify_MonsterAttackTrace::UAnimNotify_MonsterAttackTrace()
  {
  }

void UAnimNotify_MonsterAttackTrace::Notify(USkeletalMeshComponent* MeshComp,
	  UAnimSequenceBase* Animation,
	  const FAnimNotifyEventReference& EventReference)
  {
  	Super::Notify(MeshComp, Animation, EventReference);

  	if (!MeshComp)
  	{
  		return;
  	}

  	AActor* Owner = MeshComp->GetOwner();
  	if (!Owner)
  	{
  		return;
  	}

  	
  	if (Owner->Implements<UMonsterCombatInterface>())
  	{
  		IMonsterCombatInterface::Execute_ExecuteAttackTrace(Owner, AttackBoneName);
  	}
  }

FString UAnimNotify_MonsterAttackTrace::GetNotifyName_Implementation() const
  {
  	return TEXT("Monster Attack Trace");
  }