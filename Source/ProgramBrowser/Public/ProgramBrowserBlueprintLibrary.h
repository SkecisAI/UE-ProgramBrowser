// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ProgramBrowserBlueprintLibrary.generated.h"

/**
 * 
 */
UCLASS()
class PROGRAMBROWSER_API UProgramBrowserBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static bool BuildProgram(const FString& Commandline, const FString& ProgramName);

	static void GetProgramAdditionalDependenciesDirs(TArray<FString>& DependenciesDirs);

	static void StageProgram(const FString& ProgramName, const FString& ProgramTargetName, const FString& ProgramPakFile, const FString& StageDir);
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
