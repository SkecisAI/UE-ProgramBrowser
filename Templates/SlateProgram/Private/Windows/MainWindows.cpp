// Copyright Epic Games, Inc. All Rights Reserved.


#include "PROGRAM_NAMEApp.h"
#include "Windows/WindowsHWrapper.h"


/**
 * WinMain, called when the application is started
 */
int WINAPI WinMain( _In_ HINSTANCE hInInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR, _In_ int nCmdShow )
{
	// do the slate viewer thing
	RunPROGRAM_NAME(GetCommandLineW());

	return 0;
}
