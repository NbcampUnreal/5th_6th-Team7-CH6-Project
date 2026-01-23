#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

struct NECROMANCER_API FMonsterGameplayTags
{
public:
	static const FMonsterGameplayTags& Get() { return GameplayTags; }
	static void InitializeNativeTags();

	// 몬스터 상태 태그
	FGameplayTag Monster_State_Idle;
	FGameplayTag Monster_State_Combat;
	FGameplayTag Monster_State_Patrol;
	FGameplayTag Monster_State_Stagger;
	FGameplayTag Monster_State_Groggy;

	// 몬스터 타입 태그
	FGameplayTag Monster_Type_Normal;
	FGameplayTag Monster_Type_Elite;
	FGameplayTag Monster_Type_Boss;

	// 몬스터 어빌리티 태그
	FGameplayTag Monster_Ability_Attack_Melee;
	FGameplayTag Monster_Ability_Attack_Ranged;

	// 몬스터 동작 상태 태그 (Ability의 ActivationOwnedTags용)
	FGameplayTag Monster_State_Attacking;

protected:
	void AddAllTags(class UGameplayTagsManager& Manager);
	void AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment);

private:
	static FMonsterGameplayTags GameplayTags;
};
