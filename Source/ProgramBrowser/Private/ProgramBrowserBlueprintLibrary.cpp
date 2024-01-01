// Fill out your copyright notice in the Description page of Project Settings.


#include "ProgramBrowserBlueprintLibrary.h"

#include "DesktopPlatformModule.h"
#include "Misc/FileHelper.h"

bool UProgramBrowserBlueprintLibrary::BuildProgram(const FString& Commandline, const FString& ProgramName)
{
	FOutputDevice& Ar = *GLog;
	
	void* PipeRead = NULL;
	void* PipeWrite = NULL;
	verify(FPlatformProcess::CreatePipe(PipeRead, PipeWrite));

	UE_LOG(LogTemp, Warning, TEXT("=================== Build Program %s Started. ==================="), *ProgramName);
            
	FProcHandle ProcHandle = FDesktopPlatformModule::Get()->InvokeUnrealBuildToolAsync(Commandline, Ar, PipeRead, PipeWrite, true);

	if (ProcHandle.IsValid())
	{
		while (FPlatformProcess::IsProcRunning(ProcHandle))
		{
			FString OutProcOutput = FPlatformProcess::ReadPipe(PipeRead);
			UE_LOG(LogTemp, Display, TEXT("%s"), *OutProcOutput);
			FPlatformProcess::Sleep(0.1f);
		}
		int32 OutReturnCode;
		bool bGotReturnCode = FPlatformProcess::GetProcReturnCode(ProcHandle, &OutReturnCode);		
		check(bGotReturnCode);

		UE_LOG(LogTemp, Warning, TEXT("=================== Build Program %s End. ==================="), *ProgramName);

		return true;
	}

	return false;
}

void UProgramBrowserBlueprintLibrary::GetProgramAdditionalDependenciesDirs(TArray<FString>& DependenciesDirs)
{
	DependenciesDirs.Add(FPaths::Combine(FPaths::EngineContentDir(), TEXT("Slate\\Common")));
	DependenciesDirs.Add(FPaths::Combine(FPaths::EngineContentDir(), TEXT("Slate\\Old")));
	DependenciesDirs.Add(FPaths::Combine(FPaths::EngineContentDir(), TEXT("Internationalization")));
	DependenciesDirs.Add(FPaths::Combine(FPaths::EngineDir(), TEXT("Shaders\\StandaloneRenderer")));
}

void UProgramBrowserBlueprintLibrary::StageProgram(const FString& ProgramName, const FString& ProgramTargetName, const FString& ProgramPakFile, const FString& StageDir)
{
	UE_LOG(LogTemp, Warning, TEXT("=================== Stage Program %s Started. ==================="), *ProgramName);
	
	TArray<FString> ExeDependencies;
	ExeDependencies.Add(TEXT("tbb.dll"));
	ExeDependencies.Add(TEXT("tbbmalloc.dll"));
	ExeDependencies.Add(FString::Printf(TEXT("%s.exe"), *ProgramTargetName));
	ExeDependencies.Add(FString::Printf(TEXT("%s.pdb"), *ProgramTargetName));

	FString Dest = FPaths::Combine(StageDir, TEXT("Engine\\Binaries\\Win64"));
	for (FString& Dependency : ExeDependencies)
	{
		IFileManager::Get().Copy(*(Dest / Dependency), *FPaths::Combine(FPaths::EngineDir(), TEXT("Binaries\\Win64"), Dependency));
	}
	IFileManager::Get().Copy(*FPaths::Combine(StageDir, TEXT("Engine\\Content"), ProgramName + TEXT(".pak")), *ProgramPakFile);

	UE_LOG(LogTemp, Warning, TEXT("=================== Stage Program %s End. ==================="), *ProgramName);
	UE_LOG(LogTemp, Warning, TEXT("=================== Package %s Finished. ==================="), *ProgramName);
}
