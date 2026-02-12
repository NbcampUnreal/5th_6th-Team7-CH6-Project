// Fill out your copyright notice in the Description page of Project Settings.


#include "AN_MonsterNextAttack.h"
#include "MonsterBase.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"

void UAN_MonsterNextAttack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner)
	{
		return;
	}

	AMonsterBase* Monster = Cast<AMonsterBase>(Owner);
	if (!Monster)
	{
		return;
	}

	
	// 거리 체크
	ACharacter* Character = Cast<ACharacter>(Owner);
	AAIController* AIC = Character ? Cast<AAIController>(Character->GetController()) : nullptr;
	if (AIC)
	{
		UBlackboardComponent* BB = AIC->GetBlackboardComponent();
		if (BB)
		{
			AActor* Target = Cast<AActor>(BB->GetValueAsObject(FName("TargetActor")));
			if (Target)
			{
				float Distance = FVector::Dist(Owner->GetActorLocation(), Target->GetActorLocation());
				if (Distance > MaxComboDistance)
				{
					return;
				}
			}
		}
	}

	// 델리게이트를 통해 BTTask에 즉시 다음 콤보 요청
	Monster->OnNextComboRequested.ExecuteIfBound();
}
