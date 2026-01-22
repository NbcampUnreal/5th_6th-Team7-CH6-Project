#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

struct NECROMANCER_API FBaseGameplayTags
{
public:
	static const FBaseGameplayTags& Get() { return GameplayTags; }
	static void InitializeNativeTags();

	// 공용 상태 태그
	FGameplayTag State_Alive;
	FGameplayTag State_Dead;
	FGameplayTag State_Stunned;

	// 공용 행동 태그
	FGameplayTag Action_Attack;
	FGameplayTag Action_Move;

	// 공용 이벤트 태그
	FGameplayTag Event_Hit;
	FGameplayTag Event_Death;

protected:
	void AddAllTags(class UGameplayTagsManager& Manager);
	void AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment);

private:
	static FBaseGameplayTags GameplayTags;
};
