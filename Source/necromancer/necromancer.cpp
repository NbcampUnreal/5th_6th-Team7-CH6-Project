// Copyright Epic Games, Inc. All Rights Reserved.

#include "necromancer.h"
#include "Modules/ModuleManager.h"
#include "GAS/Base/BaseGameplayTags.h"
#include "GAS/Monster/MonsterGameplayTags.h"
#include "GAS/Character/CharacterGameplayTags.h"

class FNecromancerGameModule : public FDefaultGameModuleImpl
{
public:
	virtual void StartupModule() override
	{
		FBaseGameplayTags::InitializeNativeTags();
		FMonsterGameplayTags::InitializeNativeTags();
		FCharacterGameplayTags::InitializeNativeTags();
	}
};

IMPLEMENT_PRIMARY_GAME_MODULE(FNecromancerGameModule, necromancer, "necromancer");

DEFINE_LOG_CATEGORY(Lognecromancer)