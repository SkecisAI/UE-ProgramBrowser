// Copyright Epic Games, Inc. All Rights Reserved.


#include "PROGRAM_NAME.h"

#include "RequiredProgramMainCPPInclude.h"

DEFINE_LOG_CATEGORY_STATIC(LogPROGRAM_NAME, Log, All);

IMPLEMENT_APPLICATION(PROGRAM_NAME, "PROGRAM_NAME");

INT32_MAIN_INT32_ARGC_TCHAR_ARGV()
{
	GEngineLoop.PreInit(ArgC, ArgV);
	UE_LOG(LogPROGRAM_NAME, Display, TEXT("Hello World"));
	FEngineLoop::AppExit();
	return 0;
}
