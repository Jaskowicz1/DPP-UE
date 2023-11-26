// Fill out your copyright notice in the Description page of Project Settings.

using System.IO;
using EpicGames.Core;
using UnrealBuildTool;

public class DppUELibrary : ModuleRules
{
	public DppUELibrary(ReadOnlyTargetRules Target) : base(Target)
	{
		Type = ModuleType.External;

		if (!Directory.Exists(DppPath))
			throw new BuildException(
				"DPP files could not be found! Please double check your installation. If the problem persists, please reach out at https://jaskowicz.xyz/");

		if (!File.Exists(DppLib))
			throw new BuildException(DppLibName +
			                         " could not be found! Please double check your installation. If the problem persists, please reach out at https://jaskowicz.xyz/");

		if (PlatformName == "Win64" && !File.Exists(DppDll))
			throw new BuildException(
				"dpp.dll could not be found! Please double check your installation. If the problem persists, please reach out at https://jaskowicz.xyz/");
		
		PublicIncludePaths.Add(DppPath);
        
        bEnableExceptions = true;
        
		if (Target.Platform == UnrealTargetPlatform.Win64)
		{

			// If we're on debug, we want to make sure that we're actually stating debug, otherwise preprocessor won't know.
			// This is because DPP is strict on this, also because we don't want to export release dlls in debug (because it won't work).

/*
			bool debug = Target.Configuration == UnrealTargetConfiguration.Debug || 
			             Target.Configuration == UnrealTargetConfiguration.DebugGame ||
			             Target.bDebugBuildsActuallyUseDebugCRT;

			if (debug)
				PublicDefinitions.Add("_DEBUG");
			else
				PublicDefinitions.Remove("_DEBUG");
				*/
			
			// Add the import library
			PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Win64", ConfigType, "dpp.lib"));

			// Delay-load the DLL, so we can load it from the right place first
			PublicDelayLoadDLLs.Add("libcrypto-1_1-x64.dll");
			PublicDelayLoadDLLs.Add("libsodium.dll");
			PublicDelayLoadDLLs.Add("libssl-1_1-x64.dll");
			PublicDelayLoadDLLs.Add("opus.dll");
			PublicDelayLoadDLLs.Add("zlib1.dll");
			PublicDelayLoadDLLs.Add("dpp.dll");

			// Ensure that the DLL is staged along with the executable
			RuntimeDependencies.Add(libcryptoDll);
			RuntimeDependencies.Add(libsodiumDll);
			RuntimeDependencies.Add(libsslDll);
			RuntimeDependencies.Add(opusDll);
			RuntimeDependencies.Add(zlibDll);
			RuntimeDependencies.Add(DppDll);
		}
        else if (Target.Platform == UnrealTargetPlatform.Mac)
        {
            PublicDelayLoadDLLs.Add(Path.Combine(ModuleDirectory, "Mac", ConfigType, "libExampleLibrary.dylib"));
            RuntimeDependencies.Add("$(PluginDir)/Source/ThirdParty/DppUELibrary/Mac/Release/libExampleLibrary.dylib");
        }
        else
		{
			string DppLinuxPath = Path.Combine("$(PluginDir)", "Source", "ThirdParty", "DppUELibrary", "Linux", "Release", "libdpp.so");
			PublicAdditionalLibraries.Add(DppLinuxPath);
			PublicDelayLoadDLLs.Add(DppLinuxPath);
			RuntimeDependencies.Add(DppLinuxPath);
		}
	}
	
	private string DppPath => Path.GetFullPath(Path.Combine(ModuleDirectory, "Include"));
	private string ConfigType => (Target.Configuration == UnrealTargetConfiguration.Debug || Target.bDebugBuildsActuallyUseDebugCRT) ? "Debug" : "Release";

	private string DppLib =>
		Path.GetFullPath(Path.Combine(ModuleDirectory, PlatformName, ConfigType, DppLibName));

	private string DppDll => 
		Path.GetFullPath(Path.Combine(ModuleDirectory, "Win64", ConfigType, "dpp.dll"));

	private string libcryptoDll =>
		Path.GetFullPath(Path.Combine(ModuleDirectory, "Win64", ConfigType, "libcrypto-1_1-x64.dll"));
	
	private string libsodiumDll =>
		Path.GetFullPath(Path.Combine(ModuleDirectory, "Win64", ConfigType, "libsodium.dll"));
	
	private string libsslDll =>
		Path.GetFullPath(Path.Combine(ModuleDirectory, "Win64", ConfigType, "libssl-1_1-x64.dll"));
	
	private string opusDll =>
		Path.GetFullPath(Path.Combine(ModuleDirectory, "Win64", ConfigType, "opus.dll"));
	
	private string zlibDll =>
		Path.GetFullPath(Path.Combine(ModuleDirectory, "Win64", ConfigType, "zlib1.dll"));

	private string DppLibName
	{
		get
		{
			if (Target.Platform == UnrealTargetPlatform.Win64)
				return "dpp.lib";
			if (Target.Platform == UnrealTargetPlatform.Mac)
				return "dpp.dylib";
			if (Target.Platform == UnrealTargetPlatform.Linux)
				return "libdpp.so";
			return null;
		}
	}
	
	private string PlatformName
	{
		get 
		{
			if (Target.Platform == UnrealTargetPlatform.Win64)
				return "Win64";
			if (Target.Platform == UnrealTargetPlatform.Mac)
				return "Mac";
			if (Target.Platform == UnrealTargetPlatform.Linux)
				return "Linux";
			return null;
		}
	}
}
