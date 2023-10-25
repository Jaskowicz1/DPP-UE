// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class DppUE : ModuleRules
{
	public DppUE(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				Path.Combine(ModuleDirectory, "Public")
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				Path.Combine(ModuleDirectory, "Private")
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"DppUELibrary",
				"AudioExtensions",
				"AudioMixer",
				"Projects"
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"AudioExtensions",
				"AudioMixer",
				"Slate",
				"SlateCore",
				"Projects"
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
		
		string BaseDirectory = Path.GetFullPath(Path.Combine(ModuleDirectory, "..", "..", "Source", "ThirdParty", "DppUELibrary"));
		PublicIncludePaths.Add(Path.Combine(BaseDirectory, "Include"));
	}
}
