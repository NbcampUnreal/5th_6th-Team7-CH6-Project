#include "GAS/Base/BaseGameplayTags.h"
#include "GameplayTagsManager.h"

FBaseGameplayTags FBaseGameplayTags::GameplayTags;

void FBaseGameplayTags::InitializeNativeTags()
{
	UGameplayTagsManager& Manager = UGameplayTagsManager::Get();
	GameplayTags.AddAllTags(Manager);
}

void FBaseGameplayTags::AddAllTags(UGameplayTagsManager& Manager)
{
	// 공용 상태 태그
	AddTag(State_Alive, "State.Alive", "Alive");
	AddTag(State_Dead, "State.Dead", "Dead");
	AddTag(State_Stunned, "State.Stunned", "Stunned");

	// 공용 행동 태그
	AddTag(Action_Attack, "Action.Attack", "Attacking");
	AddTag(Action_Move, "Action.Move", "Moving");

	// 공용 이벤트 태그
	AddTag(Event_Hit, "Event.Hit", "Hit");
	AddTag(Event_Death, "Event.Death", "Death");
}

void FBaseGameplayTags::AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment)
{
	OutTag = UGameplayTagsManager::Get().AddNativeGameplayTag(FName(TagName),FString(TEXT("(Native) ")) + FString(TagComment));
}
