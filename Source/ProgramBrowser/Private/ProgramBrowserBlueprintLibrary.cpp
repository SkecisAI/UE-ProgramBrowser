// Fill out your copyright notice in the Description page of Project Settings.


#include "ProgramBrowserBlueprintLibrary.h"

#include "DesktopPlatformModule.h"
#include "ProgramModuleResource.h"

#define EXE_RESOURCE_ID 201
#define EXE_ARG_ID      202

bool UProgramBrowserBlueprintLibrary::RunUBT(const FString& Commandline)
{
	FOutputDevice& Ar = *GLog;
	
	void* PipeRead = NULL;
	void* PipeWrite = NULL;
	verify(FPlatformProcess::CreatePipe(PipeRead, PipeWrite));
            
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

		return true;
	}

	return false;
}

void UProgramBrowserBlueprintLibrary::GetProgramAdditionalDependenciesDirs(TArray<FString>& DependenciesDirs)
{
	// DependenciesDirs.Add(FPaths::Combine(FPaths::EngineContentDir(), TEXT("Slate\\Common")));
	// DependenciesDirs.Add(FPaths::Combine(FPaths::EngineContentDir(), TEXT("Slate\\Old")));
	// DependenciesDirs.Add(FPaths::Combine(FPaths::EngineContentDir(), TEXT("Editor\\Slate\\Icons")));
	DependenciesDirs.Add(FPaths::Combine(FPaths::EngineContentDir(), TEXT("Slate")));
	DependenciesDirs.Add(FPaths::Combine(FPaths::EngineContentDir(), TEXT("Editor")));
	
	DependenciesDirs.Add(FPaths::Combine(FPaths::EngineContentDir(), TEXT("Internationalization")));
	DependenciesDirs.Add(FPaths::Combine(FPaths::EngineDir(), TEXT("Shaders\\StandaloneRenderer")));
}

void UProgramBrowserBlueprintLibrary::StageProgram(const FString& ProgramName, const FString& ProgramTargetName,
	const FString& ProgramPakFile, const FString& StageDir, const FString& IcoPath)
{
	IFileManager::Get().DeleteDirectory(*StageDir, false, true);

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
	IFileManager::Get().Move(*FPaths::Combine(StageDir, TEXT("Engine\\Content\\Paks"), ProgramName + TEXT(".pak")), *ProgramPakFile);

	FString BootstrapPath = FPaths::Combine(FPaths::EngineDir(), TEXT("Binaries\\Win64\\BootstrapPackagedGame-Win64-Shipping.exe"));
	FString ProgramBootstrapPath = FPaths::Combine(StageDir, ProgramName + TEXT(".exe"));
	IFileManager::Get().Copy(*ProgramBootstrapPath, *BootstrapPath);

	if (!IFileManager::Get().FileExists(*BootstrapPath))
	{
		UE_LOG(LogTemp, Error, TEXT("Stage program failed! %s doesn't exist!"), *BootstrapPath);
		return;
	}

	TSharedPtr<FProgramModuleResource> ProgramModuleResource = MakeShared<FProgramModuleResource>(ProgramBootstrapPath, false);
	FString RealExePath = TEXT("Engine\\Binaries\\Win64\\") + ProgramTargetName + TEXT(".exe") + TEXT("\0");
	FString ExeArg = ProgramName + TEXT("\0");

	ProgramModuleResource->SetData(EXE_RESOURCE_ID, const_cast<TCHAR*>(*RealExePath), RealExePath.Len() * sizeof(TCHAR));
	ProgramModuleResource->SetData(EXE_ARG_ID, const_cast<TCHAR*>(*ExeArg), ExeArg.Len() * sizeof(TCHAR));

	TArray<uint8> GroupData;
	TArray<uint8> IcoData;

	if (GetIcoData(IcoPath, GroupData, IcoData))
	{
		ProgramModuleResource->SetIcon(GroupData, IcoData);
	}
}

bool UProgramBrowserBlueprintLibrary::GetIcoData(const FString& IcoPath, TArray<uint8>& OutGroupData,
                                                 TArray<uint8>& OutIcosData)
{
	TArray64<uint8> FileData;
	if (!FFileHelper::LoadFileToArray(FileData, *IcoPath, FILEREAD_Silent))
	{
		return false;
	}

	FIconDir* IconHeader = (FIconDir*)(FileData.GetData());
	FIconDirEntry* IconDirEntry = IconHeader->idEntries;

	OutIcosData.SetNumUninitialized(IconDirEntry->dwBytesInRes);
	FMemory::Memcpy(OutIcosData.GetData(), FileData.GetData() + IconDirEntry->dwImageOffset, IconDirEntry->dwBytesInRes);
	
	FMemoryWriter Writer(OutGroupData);

	// header
	Writer << IconHeader->idReserved;
	Writer << IconHeader->idType;
	Writer << IconHeader->idCount;

	// ico
	Writer << IconDirEntry->bWidth;
	Writer << IconDirEntry->bHeight;
	Writer << IconDirEntry->bColorCount;
	byte Zero = 0;
	Writer << Zero;
	Writer << IconDirEntry->wPlanes;
	Writer << IconDirEntry->wBitCount;
	Writer << IconDirEntry->dwBytesInRes;
	USHORT Index = 1;
	Writer << Index;

	return true;
}
