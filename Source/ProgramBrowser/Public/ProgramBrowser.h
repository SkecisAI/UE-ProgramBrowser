// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Features/IPluginsEditorFeature.h"
#include "Modules/ModuleManager.h"

class FProgramBrowserModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	void Initilize();

	TSharedRef<SDockTab> HandleSpawnProgramBrowserTab(const FSpawnTabArgs& SpawnTabArgs);

	static const FName ProgramBrowserEditorTabName;
	static const FName ProgramBrowserCreatorTabName;

	// Programs's path created
	static const FString NewProgramsDir;

	// Program templates path
	static FString PluginDir;
};
