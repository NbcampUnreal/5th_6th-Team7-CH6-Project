// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_MonsterMeleeAttack.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "Interface/MonsterCombatInterface.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GAS/Monster/MonsterGameplayTags.h"


UGA_MonsterMeleeAttack::UGA_MonsterMeleeAttack()
{
	//액터 하나당 하나의 인스턴스
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	//클라이언트에서 먼저 시작하고 서버검증
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	// StateTree에서 태그로 어빌리티를 찾기 위해 태그 설정
	const FMonsterGameplayTags& MonsterTags = FMonsterGameplayTags::Get();
	AbilityTags.AddTag(MonsterTags.Monster_Ability_Attack_Melee);
}

void UGA_MonsterMeleeAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo,true,true);
		return;
	}
	
	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	if (!Character||!AtteckMontage)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true,true);
		return;
	}
	
	//공격 시작
	if (Character->Implements<UMonsterCombatInterface>())
	{
		IMonsterCombatInterface::Execute_OnAttackStarted(Character);
	}
	
	//몽타주 재생
	UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Play(AtteckMontage);
		
		//몽타주 종료 바인딩
		FOnMontageEnded EndDelegate;
		EndDelegate.BindUObject(this,&UGA_MonsterMeleeAttack::OnMontageCompleted);
		AnimInstance->Montage_SetEndDelegate(EndDelegate,AtteckMontage);
	}
	
}
void UGA_MonsterMeleeAttack::EndAbility(const FGameplayAbilitySpecHandle Handle,const FGameplayAbilityActorInfo* ActorInfo,const FGameplayAbilityActivationInfo ActivationInfo,bool bReplicateEndAbility,bool bWasCancelled)
  {
        ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
        if (Character && Character->Implements<UMonsterCombatInterface>())
        {
                IMonsterCombatInterface::Execute_OnAttackEnded(Character, bWasCancelled);
        }

        Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
  }

  void UGA_MonsterMeleeAttack::OnMontageCompleted(UAnimMontage* Montage, bool bInterrupted)
  {
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, bInterrupted);
  }

  void UGA_MonsterMeleeAttack::PerformAttackTrace()
  {
        AActor* AvatarActor = GetAvatarActorFromActorInfo();
        if (!AvatarActor)
        {
                return;
        }

        FVector Start = AvatarActor->GetActorLocation();
        FVector End = Start + AvatarActor->GetActorForwardVector() * AttackTraceDistance;

        TArray<FHitResult> HitResults;
        TArray<AActor*> ActorsToIgnore;
        ActorsToIgnore.Add(AvatarActor);

        bool bHit = UKismetSystemLibrary::SphereTraceMulti(GetWorld(),Start,End,AttackTraceRadius,UEngineTypes::ConvertToTraceType(ECC_Pawn),false,ActorsToIgnore,EDrawDebugTrace::ForDuration,HitResults,true,FLinearColor::Red,FLinearColor::Green,2.0f
        );

        if (bHit)
        {
                TSet<AActor*> DamagedActors;
                for (const FHitResult& Hit : HitResults)
                {
                        AActor* HitActor = Hit.GetActor();
                        if (HitActor && !DamagedActors.Contains(HitActor))
                        {
                                DamagedActors.Add(HitActor);
                                ApplyDamageToTarget(HitActor);
                        }
                }
        }
  }

void UGA_MonsterMeleeAttack::ApplyDamageToTarget(AActor* Target)
{
	if (!Target || !DamageEffectClass)
	{
		return;
	}

	UAbilitySystemComponent* TargetASC = nullptr;

	
	if (IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(Target))
	{
		TargetASC = ASCInterface->GetAbilitySystemComponent();
	}

	if (TargetASC)
	{
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, GetAbilityLevel());
		if (SpecHandle.IsValid())
		{
			
			UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
			if (SourceASC)
			{
				SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
			}
		}
	}
}