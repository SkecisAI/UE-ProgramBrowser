// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProgramBrowser.h"

#include "ProgramData.h"
#include "SNewProgramWizard.h"
#include "SProgramBrowser.h"
#include "ToolMenus.h"
#include "WorkspaceMenuStructure.h"
#include "WorkspaceMenuStructureModule.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/FileHelper.h"
#include "Widgets/Docking/SDockTab.h"

#define LOCTEXT_NAMESPACE "FProgramBrowserModule"

const FName FProgramBrowserModule::ProgramBrowserEditorTabName = TEXT("ProgramsEditor");
const FName FProgramBrowserModule::ProgramBrowserCreatorTabName = TEXT("ProgramCreator");

FString FProgramBrowserModule::PluginDir;
FString FProgramBrowserModule::ProgramsDir;
FString FProgramBrowserModule::ProgramTemplatesDir;

void FProgramBrowserModule::StartupModule()
{
	InitilizeBrowserData();
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(ProgramBrowserEditorTabName,
		FOnSpawnTab::CreateRaw(this, &FProgramBrowserModule::HandleSpawnProgramBrowserTab))
		.SetGroup(WorkspaceMenu::GetMenuStructure().GetToolsCategory())
		.SetDisplayName(LOCTEXT("ProgramsEditorTitle", "ProgramsBrowser"))
		.SetTooltipText(LOCTEXT("ProgramsEditorTooltip", "Open the programs browser tab"))
		.SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Package"))
		.SetMenuType(ETabSpawnerMenuType::Enabled);

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(ProgramBrowserCreatorTabName,
		FOnSpawnTab::CreateRaw(this, &FProgramBrowserModule::HandleSpawnProgramCreatorTab))
		.SetDisplayName(LOCTEXT("ProgramCreatorTitle", "Create Program Wizard"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);

}

void FProgramBrowserModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(ProgramBrowserEditorTabName);
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(ProgramBrowserCreatorTabName);
}

void FProgramBrowserModule::InitilizeBrowserData()
{
	PluginDir           = IPluginManager::Get().FindPlugin("ProgramBrowser")->GetBaseDir();
	ProgramTemplatesDir = PluginDir / TEXT("Templates");
	ProgramsDir         = FPaths::EngineSourceDir() / TEXT("Programs/Programs_Collection");
	
	if (!FPaths::DirectoryExists(ProgramsDir))
	{
		IFileManager::Get().MakeDirectory(*ProgramsDir);
	}
}

TSharedRef<SDockTab> FProgramBrowserModule::HandleSpawnProgramBrowserTab(const FSpawnTabArgs& SpawnTabArgs)
{
	TSharedRef<SDockTab> DockTab = SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		.Content()
		[
			SNew(SProgramBrowser)
		];
	return DockTab;
}

TSharedRef<SDockTab> FProgramBrowserModule::HandleSpawnProgramCreatorTab(const FSpawnTabArgs& SpawnTabArgs)
{
	TSharedRef<SDockTab> DockTab = SNew(SDockTab)
	.TabRole(ETabRole::NomadTab)
	.Content()
	[
		SNew(SNewProgramWizard)
	];
	return DockTab;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FProgramBrowserModule, ProgramBrowser)