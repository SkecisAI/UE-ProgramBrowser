// Copyright Epic Games, Inc. All Rights Reserved.

#include "MySlateApp.h"
#include "RequiredProgramMainCPPInclude.h"
#include "Widgets/Docking/SDockTab.h"
#include "Framework/Application/SlateApplication.h"


IMPLEMENT_APPLICATION(PROGRAM_NAME, "PROGRAM_NAME");

#define LOCTEXT_NAMESPACE "PROGRAM_NAME"

void RestoreSlate()
{
	auto SpawnGallery = [](const FSpawnTabArgs&) -> TSharedRef<SDockTab>
	{
		return SNew(SDockTab)
			.TabRole(ETabRole::NomadTab)
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("Hello World!")))
			];
	};
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner("My Slate Program", FOnSpawnTab::CreateLambda(SpawnGallery));
	
	TSharedRef<FTabManager::FLayout> Layout = FTabManager::NewLayout( "MySlateProgram_Layout" )
	->AddArea
	(
		FTabManager::NewArea(1230, 900)
		->Split
		(
			FTabManager::NewStack()
			->AddTab("My Slate Program", ETabState::OpenedTab)
			->SetForegroundTab(FName("My Slate Program"))
		)
	);

	FGlobalTabmanager::Get()->RestoreFrom(Layout, TSharedPtr<SWindow>());
}

int RunPROGRAM_NAME( const TCHAR* CommandLine )
{
	FTaskTagScope TaskTagScope(ETaskTag::EGameThread);	

	// start up the main loop
	GEngineLoop.PreInit(CommandLine);

	// Make sure all UObject classes are registered and default properties have been initialized
	ProcessNewlyLoadedUObjects();
	
	// Tell the module manager it may now process newly-loaded UObjects when new C++ modules are loaded
	FModuleManager::Get().StartProcessingNewlyLoadedObjects();


	// crank up a normal Slate application using the platform's standalone renderer
	FSlateApplication::InitializeAsStandaloneApplication(GetStandardStandaloneRenderer());

	FSlateApplication::InitHighDPI(true);

	// set the application name
	FGlobalTabmanager::Get()->SetApplicationTitle(LOCTEXT("AppTitle", "My Slate"));

	RestoreSlate();
	
	// loop while the server does the rest
	while (!IsEngineExitRequested())
	{
		BeginExitIfRequested();

		FTaskGraphInterface::Get().ProcessThreadUntilIdle(ENamedThreads::GameThread);
		FStats::AdvanceFrame(false);
		FTSTicker::GetCoreTicker().Tick(FApp::GetDeltaTime());
		FSlateApplication::Get().PumpMessages();
		FSlateApplication::Get().Tick();		
		FPlatformProcess::Sleep(0.01);
	
		GFrameCounter++;
	}

	FCoreDelegates::OnExit.Broadcast();
	FSlateApplication::Shutdown();
	FModuleManager::Get().UnloadModulesAtShutdown();

	GEngineLoop.AppPreExit();
	GEngineLoop.AppExit();

	return 0;
}

#undef LOCTEXT_NAMESPACE
