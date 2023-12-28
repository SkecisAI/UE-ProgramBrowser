// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProgramBrowser.h"

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
const FString FProgramBrowserModule::NewProgramsDir = FPaths::Combine(FPaths::EngineSourceDir(), TEXT("Programs/ProgramBrowserExplorer"));
FString FProgramBrowserModule::PluginDir;

void FProgramBrowserModule::StartupModule()
{
	Initilize();
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(ProgramBrowserEditorTabName,
		FOnSpawnTab::CreateRaw(this, &FProgramBrowserModule::HandleSpawnProgramBrowserTab))
		.SetGroup(WorkspaceMenu::GetMenuStructure().GetToolsCategory())
		.SetDisplayName(LOCTEXT("ProgramsEditorTitle", "ProgramsBrowser"))
		.SetTooltipText(LOCTEXT("ProgramsEditorTooltip", "Open the programs browser tab"))
		.SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Package"))
		.SetMenuType(ETabSpawnerMenuType::Enabled);

}

void FProgramBrowserModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

void FProgramBrowserModule::Initilize()
{
	if (!FPaths::DirectoryExists(NewProgramsDir))
	{
		IFileManager::Get().MakeDirectory(*NewProgramsDir);
	}

	PluginDir = IPluginManager::Get().FindPlugin("ProgramBrowser")->GetBaseDir();
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

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FProgramBrowserModule, ProgramBrowser)