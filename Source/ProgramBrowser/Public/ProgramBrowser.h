// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Features/IPluginsEditorFeature.h"
#include "Modules/ModuleManager.h"

struct FProgram;
struct FNewProgramTemplate;

class FProgramBrowserModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	void InitilizeBrowserData();

	TSharedRef<SDockTab> HandleSpawnProgramBrowserTab(const FSpawnTabArgs& SpawnTabArgs);

	TSharedRef<SDockTab> HandleSpawnProgramCreatorTab(const FSpawnTabArgs& SpawnTabArgs);

	static const FName ProgramBrowserEditorTabName;
	static const FName ProgramBrowserCreatorTabName;

	// this plugin path
	static FString PluginDir;

	static FString ProgramTemplatesDir;

	static FString ProgramsDir;

private:
	TArray<TSharedRef<FNewProgramTemplate>> ProgramTemplates;
};
