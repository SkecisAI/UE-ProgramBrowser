// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Misc/FileHelper.h"
#include "ProgramBrowserBlueprintLibrary.generated.h"


#pragma pack(push,1)
struct FIconDirEntry
{
	uint8 bWidth;			// Width, in pixels, of the image
	uint8 bHeight;			// Height, in pixels, of the image
	uint8 bColorCount;		// Number of colors in image (0 if >=8bpp)
	uint8 bReserved;		// Reserved ( must be 0)
	uint16 wPlanes;			// Color Planes
	uint16 wBitCount;		// Bits per pixel
	uint32 dwBytesInRes;	// How many bytes in this resource?
	uint32 dwImageOffset;	// Where in the file is this image?
};
#pragma pack(pop)


#pragma pack(push,1)
struct FIconDir
{
	uint16 idReserved;			// Reserved (must be 0)
	uint16 idType;				// Resource Type (1 for icons)
	uint16 idCount;				// How many images?
	FIconDirEntry idEntries[1];	// An entry for each image (idCount of 'em)
};
#pragma pack(pop)

/**
 * 
 */
UCLASS()
class PROGRAMBROWSER_API UProgramBrowserBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static bool RunUBT(const FString& Commandline);

	static void GetProgramAdditionalDependenciesDirs(TArray<FString>& DependenciesDirs);

	static void StageProgram(const FString& ProgramName, const FString& ProgramTargetName,
		const FString& ProgramPakFile, const FString& StageDir, const FString& IcoPath);

	static bool GetIcoData(const FString& IcoPath, TArray<uint8>& OutGroupData, TArray<uint8>& OutIcosData);
};


class FProgramBrowserWorker : public FRunnable
{
public:
	using FCallback = TFunction<void()>;
	
	FProgramBrowserWorker(const FString& InThreadName, const FCallback& InFunction)
		:
	ThreadName(InThreadName),
	Function(InFunction)
	{
		Thread = FRunnableThread::Create(this, *ThreadName);
		check(Thread);
	}

	~FProgramBrowserWorker()
	{
		if (Thread != nullptr)
		{
			Thread->Kill(true);
			delete Thread;
		}
	}

	virtual uint32 Run() override
	{
		Function();

		return 0;
	}

	void Join()
	{
		if (Thread)
		{
			Thread->WaitForCompletion();
		}
	}

private:
	FString ThreadName;
	FCallback Function;

	FRunnableThread* Thread;
};

namespace ProgramUtils
{
	const FString PROGRAM_NAME = TEXT("PROGRAM_NAME");
	class FCopyProgramFileAndDirs : public IPlatformFile::FDirectoryVisitor
	{
	private:
		IPlatformFile& PlatformFile;
		const FString SourceRoot;
		const FString DestRoot;
		const FString& ProgramName;
		TArray<FString> NameReplacementFileTypes;
		TArray<FString> IgnoreFileTypes;
	public:
		FCopyProgramFileAndDirs(IPlatformFile& InPlatforFile, const FString& InSourceRoot, const FString& InDestRoot,
			const FString& InProgramName)
			: PlatformFile(InPlatforFile)
			, SourceRoot(InSourceRoot)
			, DestRoot(InDestRoot)
			, ProgramName(InProgramName)
		{
			NameReplacementFileTypes.Add(TEXT("cs"));
			NameReplacementFileTypes.Add(TEXT("cpp"));
			NameReplacementFileTypes.Add(TEXT("h"));
			NameReplacementFileTypes.Add(TEXT("vcxproj"));
		}

		virtual bool Visit(const TCHAR* FilenameOrDirectory, bool bIsDirectory) override
		{
			FString NewName(FilenameOrDirectory);
			NewName.RemoveFromStart(SourceRoot);
			NewName = NewName.Replace(*PROGRAM_NAME, *ProgramName, ESearchCase::CaseSensitive);
			NewName = FPaths::Combine(DestRoot, *NewName);

			if (bIsDirectory)
			{
				if (!PlatformFile.CreateDirectoryTree(*NewName) && !PlatformFile.DirectoryExists(*NewName))
				{
					return false;
				}
			}
			else
			{
				FString Ext = FPaths::GetExtension(FilenameOrDirectory);

				if (NameReplacementFileTypes.Contains(Ext))
				{
					FString FileContent;
					if (!FFileHelper::LoadFileToString(FileContent, FilenameOrDirectory))
					{
						return false;
					}
					
					FileContent = FileContent.Replace(*PROGRAM_NAME, *ProgramName, ESearchCase::CaseSensitive);

					FString ProgramNameAPI = ProgramName + TEXT("_API");
					FileContent = FileContent.Replace(*ProgramNameAPI, *ProgramNameAPI.ToUpper(), ESearchCase::CaseSensitive);

					if (!FFileHelper::SaveStringToFile(FileContent, *NewName))
					{
						return false;
					}
				}
				else
				{
					if (!PlatformFile.CopyFile(*NewName, FilenameOrDirectory))
					{
						return false;
					}
				}

			}
			
			return true;
		}
	};
}