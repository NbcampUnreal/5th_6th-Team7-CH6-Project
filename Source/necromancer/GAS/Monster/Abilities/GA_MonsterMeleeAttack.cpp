// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_MonsterMeleeAttack.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "Interface/MonsterCombatInterface.h"
#include "Kismet/KismetSystemLibrary.h"


UGA_MonsterMeleeAttack::UGA_MonsterMeleeAttack()
{
	//액터 하나당 하나의 인스턴스
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	//클라이언트에서 먼저 시작하고 서버검증
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	// === GAS 태그 설정 (공식 권장 패턴) ===

	// 1. Ability Tags - 이 능력의 식별자 (TryActivateAbilitiesByTag에서 사용)
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Monster.Ability.Attack.Melee")));

	// 2. Activation Owned Tags - 활성화 중 ASC에 자동으로 추가되는 태그
	//    EndAbility 호출 시 자동으로 제거됨 → StateTree에서 이 태그로 상태 확인 가능!
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Monster.State.Attacking")));

	// 3. Activation Blocked Tags - 이 태그가 있으면 활성화 차단
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Dead")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Monster.State.Attacking"))); // 연속 공격 방지

	// 4. Block Abilities With Tags - 활성화 중 다른 공격 능력 차단
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(FName("Monster.Ability.Attack")));
}

void UGA_MonsterMeleeAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	if (!Character || !AttackMontage)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 공격 시작
	if (Character->Implements<UMonsterCombatInterface>())
	{
		IMonsterCombatInterface::Execute_OnAttackStarted(Character);
	}

	// 몽타주 재생
	UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Play(AttackMontage);

		// 몽타주 종료 바인딩
		FOnMontageEnded EndDelegate;
		EndDelegate.BindUObject(this, &UGA_MonsterMeleeAttack::OnMontageCompleted);
		AnimInstance->Montage_SetEndDelegate(EndDelegate, AttackMontage);
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