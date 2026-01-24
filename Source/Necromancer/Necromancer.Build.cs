// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Necromancer : ModuleRules
{
	public Necromancer(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"EnhancedInput",
            "AIModule",
            "NavigationSystem",
            "GameplayTasks",
            "UMG",
            "Niagara"
        });

        PublicIncludePaths.AddRange(new string[]
        {
            "Necromancer",
        });
    }
}
