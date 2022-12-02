// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class COMP3013 : ModuleRules
{
	public COMP3013(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "UMG", "HeadMountedDisplay", "NavigationSystem", "AIModule", "Niagara" });
        PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
	}
}
