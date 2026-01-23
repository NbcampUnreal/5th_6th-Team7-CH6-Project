// Fill out your copyright notice in the Description page of Project Settings.

#include "GA_MonsterDeath.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "GAS/Monster/MonsterGameplayTags.h"
#include "GAS/Base/BaseGameplayTags.h"

UGA_MonsterDeath::UGA_MonsterDeath()
{
	// 액터 하나당 하나의 인스턴스
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	// 서버에서 시작
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;

	// Ability 식별 태그
	const FMonsterGameplayTags& MonsterTags = FMonsterGameplayTags::Get();
	FGameplayTagContainer Tags;
	Tags.AddTag(MonsterTags.Monster_Ability_Death);
	SetAssetTags(Tags);

	// 활성화 중 ASC에 추가될 태그 (사망 애니메이션 재생 중)
	ActivationOwnedTags.AddTag(MonsterTags.Monster_State_Dying);

	// State.Dead가 이미 있으면 활성화 차단 (중복 사망 방지)
	const FBaseGameplayTags& BaseTags = FBaseGameplayTags::Get();
	ActivationBlockedTags.AddTag(BaseTags.State_Dead);

	// Event.Death 이벤트로 자동 트리거 설정
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = BaseTags.Event_Death;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);
}

void UGA_MonsterDeath::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	if (!Character)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// State.Dead 태그 즉시 추가
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC)
	{
		const FBaseGameplayTags& BaseTags = FBaseGameplayTags::Get();
		ASC->AddLooseGameplayTag(BaseTags.State_Dead);

		// 다른 모든 Ability 취소 (현재 Ability 제외)
		ASC->CancelAllAbilities();
	}

	// 콜리전 및 이동 비활성화
	Character->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Character->GetCharacterMovement()->DisableMovement();

	UE_LOG(LogTemp, Warning, TEXT("[%s] === Monster Death Ability Activated! ==="), *Character->GetName());

	// 몽타주가 있으면 재생
	if (DeathMontage)
	{
		UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->Montage_Play(DeathMontage);

			// 몽타주 종료 바인딩
			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(this, &UGA_MonsterDeath::OnDeathMontageCompleted);
			AnimInstance->Montage_SetEndDelegate(EndDelegate, DeathMontage);

			UE_LOG(LogTemp, Log, TEXT("[%s] Death montage started"), *Character->GetName());
			return; // 몬타주 완료 대기
		}
	}

	// 몽타주 없으면 즉시 물리화
	ActivateRagdoll();
}

void UGA_MonsterDeath::OnDeathMontageCompleted(UAnimMontage* Montage, bool bInterrupted)
{
	UE_LOG(LogTemp, Log, TEXT("Death montage completed (Interrupted: %s)"), bInterrupted ? TEXT("Yes") : TEXT("No"));
	ActivateRagdoll();
}

void UGA_MonsterDeath::ActivateRagdoll()
{
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (Character)
	{
		// Ragdoll 물리화
		Character->GetMesh()->SetSimulatePhysics(true);
		Character->GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

		// 액터 제거 설정
		Character->SetLifeSpan(DeathRemovalTime);

		UE_LOG(LogTemp, Warning, TEXT("[%s] Ragdoll activated, will be destroyed in %.1f seconds"),
			*Character->GetName(), DeathRemovalTime);
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
