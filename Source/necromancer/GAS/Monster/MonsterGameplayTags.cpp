#include "GAS/Monster/MonsterGameplayTags.h"
#include "GameplayTagsManager.h"

FMonsterGameplayTags FMonsterGameplayTags::GameplayTags;

void FMonsterGameplayTags::InitializeNativeTags()
{
	UGameplayTagsManager& Manager = UGameplayTagsManager::Get();
	GameplayTags.AddAllTags(Manager);
}

void FMonsterGameplayTags::AddAllTags(UGameplayTagsManager& Manager)
{
	// 몬스터 상태 태그
	AddTag(Monster_State_Idle, "Monster.State.Idle", "Idle");
	AddTag(Monster_State_Combat, "Monster.State.Combat", "Combat");
	AddTag(Monster_State_Patrol, "Monster.State.Patrol", "Patrol");
	AddTag(Monster_State_Stagger, "Monster.State.Stagger", "Stagger");
	AddTag(Monster_State_Groggy, "Monster.State.Groggy", "Groggy");

	// 몬스터 타입 태그
	AddTag(Monster_Type_Normal, "Monster.Type.Normal", "Normal");
	AddTag(Monster_Type_Elite, "Monster.Type.Elite", "Elite");
	AddTag(Monster_Type_Boss, "Monster.Type.Boss", "Boss");

	// 몬스터 어빌리티 태그
	AddTag(Monster_Ability_Attack_Melee, "Monster.Ability.Attack.Melee", "Melee Attack");
	AddTag(Monster_Ability_Attack_Ranged, "Monster.Ability.Attack.Ranged", "Ranged Attack");

	// 몬스터 동작 상태 태그 (Ability의 ActivationOwnedTags용)
	AddTag(Monster_State_Attacking, "Monster.State.Attacking", "Monster is currently attacking");
}

void FMonsterGameplayTags::AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment)
{
	OutTag = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TagName),
		FString(TEXT("(Native) ")) + FString(TagComment)
	);
}
