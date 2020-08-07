// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class FloodPedestrian : ModuleRules
{
	public FloodPedestrian(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "UMG", "ProceduralMeshComponent" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Uncomment if you are using Slate UI
		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        string dir = Path.GetFullPath(Path.Combine(ModuleDirectory, "../../glm/glm/detail"));
        PublicIncludePaths.Add(dir);
        dir = Path.GetFullPath(Path.Combine(ModuleDirectory, "../../glm/glm"));
        PublicIncludePaths.Add(dir);

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
