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

// Called when the game starts or when spawned
void ANec_SkeletonMeleeMonster::BeginPlay()
{
	Super::BeginPlay();
	InitializeAbilities();
}

void ANec_SkeletonMeleeMonster::InitializeAbilities()
{
	if (!AbilitySystemComponent || !MeleeAttackAbilityClass)
	{
		return;
	}

	FGameplayAbilitySpec AbilitySpec(MeleeAttackAbilityClass, 1, INDEX_NONE, this);
	AbilitySystemComponent->GiveAbility(AbilitySpec);
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

	const FMonsterGameplayTags& MonsterTags = FMonsterGameplayTags::Get();
	TArray<FGameplayAbilitySpec*> AbilitySpecs;
	FGameplayTagContainer AttackTags;
	AttackTags.AddTag(MonsterTags.Monster_Ability_Attack_Melee);
	AbilitySystemComponent->GetActivatableGameplayAbilitySpecsByAllMatchingTags(AttackTags, AbilitySpecs);

	for (FGameplayAbilitySpec* Spec : AbilitySpecs)
	{
		if (Spec && Spec->IsActive())
		{
			UGA_MonsterMeleeAttack* AttackAbility = Cast<UGA_MonsterMeleeAttack>(Spec->GetPrimaryInstance());
			if (AttackAbility)
			{
				AttackAbility->PerformAttackTrace();
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
		// Dead 태그 추가
		const FBaseGameplayTags& BaseTags = FBaseGameplayTags::Get();
		AbilitySystemComponent->AddLooseGameplayTag(BaseTags.State_Dead);

		AbilitySystemComponent->CancelAllAbilities();
	}

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->DisableMovement();

	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	SetLifeSpan(5.0f);
}

bool ANec_SkeletonMeleeMonster::IsAlive_Implementation() const
{
	return !bIsDead;
}

