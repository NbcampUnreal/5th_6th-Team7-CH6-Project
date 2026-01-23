#include "GAS/Character/CharacterGameplayTags.h"
#include "GameplayTagsManager.h"

FCharacterGameplayTags FCharacterGameplayTags::GameplayTags;

void FCharacterGameplayTags::InitializeNativeTags()
{
	UGameplayTagsManager& Manager = UGameplayTagsManager::Get();
	GameplayTags.AddAllTags(Manager);
}

void FCharacterGameplayTags::AddAllTags(UGameplayTagsManager& Manager)
{
	AddTag(Character_State_Idle, "Character.State.Idle", "Idle");
	AddTag(Character_State_Combat, "Character.State.Combat", "Combat");
	AddTag(Character_State_Patrol, "Character.State.Patrol", "Patrol");
	AddTag(Character_State_Stagger, "Character.State.Stagger", "Stagger");
	AddTag(Character_State_Groggy, "Character.State.Groggy", "Groggy");

	AddTag(Character_Ability_Attack, "Character.Ability.Attack", "Attack");	

	AddTag(Character_Event_Montage_NextStep, "Character.Event.Montage.NextStep", "NextStep");
}

void FCharacterGameplayTags::AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment)
{
	OutTag = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TagName),
		FString(TEXT("(Native) ")) + FString(TagComment)
	);
}