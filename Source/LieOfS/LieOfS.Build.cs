// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class LieOfS : ModuleRules
{
	public LieOfS(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG", "NavigationSystem", 
			"AIModule", "GameplayTasks", "AnimGraphRuntime", "MovieScene", "LevelSequence" });

        PrivateIncludePaths.Add("LieOfS");
    }
}
