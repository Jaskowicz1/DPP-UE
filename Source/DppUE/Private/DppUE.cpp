// Copyright Epic Games, Inc. All Rights Reserved.

#include "DppUE.h"
#include "Core.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"

#define LOCTEXT_NAMESPACE "FDppUEModule"

void FDppUEModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	// Get the base directory of this plugin
	FString BaseDir = IPluginManager::Get().FindPlugin("DppUE")->GetBaseDir();

	// Add on the relative location of the third party dll and load it
	FString dppLibraryPath;
	FString libcryptoLibraryPath;
	FString libsodiumLibraryPath;
	FString libsslLibraryPath;
	FString opusLibraryPath;
	FString zlibLibraryPath;

	FString ConfigType = "Release";

#if PLATFORM_WINDOWS
	#if UE_BUILD_DEBUG
	ConfigType = "Debug";
	#endif
	#ifdef _DEBUG
	ConfigType = "Debug";
	#endif
#endif
	
	// No Mac support, I don't have a Mac PC to test nor compile this.
	
#if PLATFORM_WINDOWS
	dppLibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/DppUELibrary/Win64"), *ConfigType, TEXT("dpp.dll"));
	libcryptoLibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/DppUELibrary/Win64"), *ConfigType, TEXT("libcrypto-1_1-x64.dll"));
	libsodiumLibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/DppUELibrary/Win64"), *ConfigType, TEXT("libsodium.dll"));
	libsslLibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/DppUELibrary/Win64"), *ConfigType, TEXT("libssl-1_1-x64.dll"));
	opusLibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/DppUELibrary/Win64"), *ConfigType, TEXT("opus.dll"));
	zlibLibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/DppUELibrary/Win64"), *ConfigType, TEXT("zlib1.dll"));

	libcryptoLibraryHandle = !libcryptoLibraryPath.IsEmpty() ? FPlatformProcess::GetDllHandle(*libcryptoLibraryPath) : nullptr;
	libsodiumLibraryHandle = !libsodiumLibraryPath.IsEmpty() ? FPlatformProcess::GetDllHandle(*libsodiumLibraryPath) : nullptr;
	libsslLibraryHandle = !libsslLibraryPath.IsEmpty() ? FPlatformProcess::GetDllHandle(*libsslLibraryPath) : nullptr;
	opusLibraryHandle = !opusLibraryPath.IsEmpty() ? FPlatformProcess::GetDllHandle(*opusLibraryPath) : nullptr;
	zlibLibraryHandle = !zlibLibraryPath.IsEmpty() ? FPlatformProcess::GetDllHandle(*zlibLibraryPath) : nullptr;
	
#elif PLATFORM_LINUX
	dppLibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/DppUELibrary/Linux"), *ConfigType, TEXT("libdpp.so"));
	// Linux should already have libcrypto installed. They will NEED to install zlib and OpenSSL and any other additional libraries.
	// I don't agree with it but, for now, that's how it was to be.
#endif // PLATFORM_WINDOWS
	dppLibraryHandle = !dppLibraryPath.IsEmpty() ? FPlatformProcess::GetDllHandle(*dppLibraryPath) : nullptr;

	// Eventually just swap this out for a function that will load each library into a handle.
	// That way, we don't need the spam of #if

#if PLATFORM_WINDOWS
	if (!libcryptoLibraryHandle || !libsodiumLibraryHandle || !libsslLibraryHandle || !opusLibraryHandle || !zlibLibraryHandle || !dppLibraryHandle)
	{
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("DppUEModule", "Failed to load DPP."));
	}
#else
	if (!dppLibraryHandle)
	{
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("DppUEModule", "Failed to load DPP."));
	}
#endif
}

void FDppUEModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	// Free the dll handle
	FPlatformProcess::FreeDllHandle(dppLibraryHandle);
	FPlatformProcess::FreeDllHandle(libcryptoLibraryHandle);
	FPlatformProcess::FreeDllHandle(libsodiumLibraryHandle);
	FPlatformProcess::FreeDllHandle(libsslLibraryHandle);
	FPlatformProcess::FreeDllHandle(opusLibraryHandle);
	FPlatformProcess::FreeDllHandle(zlibLibraryHandle);
	dppLibraryHandle = nullptr;
	libcryptoLibraryHandle = nullptr;
	libsodiumLibraryHandle = nullptr;
	libsslLibraryHandle = nullptr;
	opusLibraryHandle = nullptr;
	zlibLibraryHandle = nullptr;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FDppUEModule, DppUE)
