#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

struct NECROMANCER_API FCharacterGameplayTags
{
public:
	static const FCharacterGameplayTags& Get() { return GameplayTags; }
	static void InitializeNativeTags();
		
	FGameplayTag Character_State_Idle;
	FGameplayTag Character_State_Combat;
	FGameplayTag Character_State_Patrol;
	FGameplayTag Character_State_Stagger;
	FGameplayTag Character_State_Groggy;

	FGameplayTag Character_Ability_Attack;

	FGameplayTag Character_Event_Montage_NextStep;

protected:
	void AddAllTags(class UGameplayTagsManager& Manager);
	void AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment);

private:
	static FCharacterGameplayTags GameplayTags;
};
