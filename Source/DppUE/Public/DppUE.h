// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

class FDppUEModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	/** Handle to the test dll we will load */
	void* dppLibraryHandle = nullptr;
	void* libcryptoLibraryHandle = nullptr;
	void* libsodiumLibraryHandle = nullptr;
	void* libsslLibraryHandle = nullptr;
	void* opusLibraryHandle = nullptr;
	void* zlibLibraryHandle = nullptr;
};
