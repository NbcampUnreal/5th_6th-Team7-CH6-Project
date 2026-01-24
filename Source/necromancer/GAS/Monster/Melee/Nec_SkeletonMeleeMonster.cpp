// Fill out your copyright notice in the Description page of Project Settings.


#include "Nec_SkeletonMeleeMonster.h"
#include "AbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GA_MonsterMeleeAttack.h"
#include "MonsterGameplayTags.h"
#include "GameplayTagContainer.h"
#include "BaseGameplayTags.h"

// Sets default values
ANec_SkeletonMeleeMonster::ANec_SkeletonMeleeMonster()
{
}

void ANec_SkeletonMeleeMonster::BeginPlay()
{
	Super::BeginPlay();
	InitializeAbilities();
}

void ANec_SkeletonMeleeMonster::InitializeAbilities()
{
	if (!AbilitySystemComponent)
	{
		return;
	}

	// 근접 공격 어빌리티 부여
	if (MeleeAttackAbilityClass)
	{
		FGameplayAbilitySpec AbilitySpec(MeleeAttackAbilityClass, 1, INDEX_NONE, this);
		AbilitySystemComponent->GiveAbility(AbilitySpec);
	}

	// 전투 지속 버프 Passive Ability 부여 및 즉시 활성화
	if (CombatPersistenceBuffAbilityClass)
	{
		FGameplayAbilitySpec BuffSpec(CombatPersistenceBuffAbilityClass, 1, INDEX_NONE, this);
		FGameplayAbilitySpecHandle BuffHandle = AbilitySystemComponent->GiveAbility(BuffSpec);
		AbilitySystemComponent->TryActivateAbility(BuffHandle);
	}

	// Death Ability 부여 (Event.Death 이벤트로 트리거됨)
	if (DeathAbilityClass)
	{
		FGameplayAbilitySpec DeathSpec(DeathAbilityClass, 1, INDEX_NONE, this);
		AbilitySystemComponent->GiveAbility(DeathSpec);
	}
}

bool ANec_SkeletonMeleeMonster::TryActivateMeleeAttack()
{
	if (!CanAttack_Implementation())
	{
		return false;
	}

	if (!AbilitySystemComponent)
	{
		return false;
	}

	// MonsterGameplayTags에서 태그 가져오기
	const FMonsterGameplayTags& MonsterTags = FMonsterGameplayTags::Get();
	FGameplayTagContainer AttackTags;
	AttackTags.AddTag(MonsterTags.Monster_Ability_Attack_Melee);

	return AbilitySystemComponent->TryActivateAbilitiesByTag(AttackTags);
}

void ANec_SkeletonMeleeMonster::ExecuteAttackTrace_Implementation(FName AttackBoneName)
{
	if (!AbilitySystemComponent)
	{
		return;
	}

	// 모든 Ability를 순회하여 GA_MonsterMeleeAttack 찾기
	for (const FGameplayAbilitySpec& Spec : AbilitySystemComponent->GetActivatableAbilities())
	{
		if (Spec.IsActive())
		{
			for (UGameplayAbility* Instance : Spec.GetAbilityInstances())
			{
				if (UGA_MonsterMeleeAttack* AttackAbility = Cast<UGA_MonsterMeleeAttack>(Instance))
				{
					AttackAbility->PerformAttackTrace();
				}
			}
		}
	}
}

void ANec_SkeletonMeleeMonster::OnAttackStarted_Implementation()
{
	bIsAttacking = true;
}

void ANec_SkeletonMeleeMonster::OnAttackEnded_Implementation(bool bWasInterrupted)
{
	bIsAttacking = false;
}

bool ANec_SkeletonMeleeMonster::CanAttack_Implementation() const
{
	return !bIsAttacking&&!bIsDead;
}

void ANec_SkeletonMeleeMonster::HandleDeath_Implementation()
{
	if (bIsDead)
	{
		return;
	}

	bIsDead = true;

	if (AbilitySystemComponent)
	{
		// Event.Death 이벤트로 Death Ability 트리거
		FGameplayEventData EventData;
		EventData.Instigator = this;
		EventData.Target = this;

		const FBaseGameplayTags& BaseTags = FBaseGameplayTags::Get();
		AbilitySystemComponent->HandleGameplayEvent(BaseTags.Event_Death, &EventData);
	}
}

bool ANec_SkeletonMeleeMonster::IsAlive_Implementation() const
{
	return !bIsDead;
}

