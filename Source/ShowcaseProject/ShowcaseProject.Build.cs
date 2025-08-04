// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ShowcaseProject : ModuleRules
{
	public ShowcaseProject(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AnimGraphRuntime",
			"Niagara",
			"GameplayTags",
			"AIModule",
			"GameplayTasks",
			"NavigationSystem",
			"UMG",
			"Slate",
			"SlateCore",
			"StateTreeModule",
			"GameplayStateTreeModule",
		});
	}
}
