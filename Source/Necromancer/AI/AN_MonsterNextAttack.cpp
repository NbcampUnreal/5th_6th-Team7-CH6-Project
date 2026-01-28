// Fill out your copyright notice in the Description page of Project Settings.


#include "AN_MonsterNextAttack.h"
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

	ACharacter* Character = Cast<ACharacter>(Owner);
	if (!Character)
	{
		return;
	}

	UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
	if (!AnimInstance)
	{
		return;
	}

	
	UAnimMontage* CurrentMontage = AnimInstance->GetCurrentActiveMontage();
	if (!CurrentMontage)
	{
		return;
	}

	
	int32 SectionIndex = CurrentMontage->GetSectionIndex(NextSectionName);
	if (SectionIndex == INDEX_NONE)
	{
		return;
	}

	
	bool bShouldCombo = false;

	
	if (FMath::FRand() > ComboChance)
	{
		return;
	}

	
	AAIController* AIC = Cast<AAIController>(Character->GetController());
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

	
	AnimInstance->Montage_JumpToSection(NextSectionName, CurrentMontage);
	
	
	
}
