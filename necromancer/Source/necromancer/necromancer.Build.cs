// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class necromancer : ModuleRules
{
	public necromancer(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate",
			// GAS (Gameplay Ability System) 모듈
			"GameplayAbilities",    // GAS 핵심 - Ability, Effect 사용
			"GameplayTags",         // GameplayTag 사용
			"GameplayTasks"         // GameplayTask 사용 (비동기 작업)
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"necromancer",
			"necromancer/Variant_Platforming",
			"necromancer/Variant_Platforming/Animation",
			"necromancer/Variant_Combat",
			"necromancer/Variant_Combat/AI",
			"necromancer/Variant_Combat/Animation",
			"necromancer/Variant_Combat/Gameplay",
			"necromancer/Variant_Combat/Interfaces",
			"necromancer/Variant_Combat/UI",
			"necromancer/Variant_SideScrolling",
			"necromancer/Variant_SideScrolling/AI",
			"necromancer/Variant_SideScrolling/Gameplay",
			"necromancer/Variant_SideScrolling/Interfaces",
			"necromancer/Variant_SideScrolling/UI",
			// GAS 관련 폴더
			"necromancer/GAS",
			"necromancer/GAS/Base",
			"necromancer/GAS/Monster"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
