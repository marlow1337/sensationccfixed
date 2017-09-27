/*
ApocalypseCheats
------------------------------
Contributors:
XBoom
Ma$$A$
madlifer
control1337
CyclesPrograming
FZCat1337
UC Community <3
*/
//#define AC_DEBUG

// General shit
#include "DLLMain.h"
#include "Utilities.h"

// Injection stuff
#include "ReflectiveLoader.h"

// Stuff to initialise
#include "Offsets.h"
#include "Interfaces.h"
#include "Hooks.h"
#include "RenderManager.h"
#include "Hacks.h"
#include "Hooks.h"
#include "Menu.h"
#include "AntiAntiAim.h"

#include "Dumping.h"

// Used as part of the reflective DLL injection
extern HINSTANCE hAppInstance;

// Our DLL Instance
HINSTANCE HThisModule;
bool DoUnload;

// Our thread we use to setup everything we need
// Everything appart from code in hooks get's called from inside 
// here.

int InitialThread()
{
#ifdef AC_DEBUG
	Utilities::OpenConsole("Logic");
#endif
	// Intro banner with info
	PrintMetaHeader();

	//---------------------------------------------------------
	// Initialise all our shit
	Offsets::Initialise(); // Set our VMT offsets and do any pattern scans
	Interfaces::Initialise(); // Get pointers to the valve classes
	NetVar.RetrieveClasses(); // Setup our NetVar manager (thanks shad0w bby)
	NetvarManager::Instance()->CreateDatabase();
	Render::Initialise();
	Hacks::SetupHacks();
	Menu::SetupMenu();
	Hooks::Initialise();
	ApplyAAAHooks();
	HooksXD::XD3();
	//GUI.LoadWindowState(&Menu::Window, "config.xml");

	// Dumping
	//Dump::DumpClassIds();

	//---------------------------------------------------------
	Utilities::Log("Ready");
	
	
	//MessageBox(NULL, "Cheat loaded succesfully in CSGO.", "Apocalypse", MB_OK);
	// While our cheat is running
	while (DoUnload == false)
	{
		Sleep(1000);
	}

	Hooks::UndoHooks();
	Sleep(2000); // Make sure none of our hooks are running
	FreeLibraryAndExitThread(HThisModule, 0);

	return 0;
}

// DllMain
// Entry point for our module
BOOL WINAPI DllMain(
	_In_ HINSTANCE hinstDLL,
	_In_ DWORD     fdwReason,
	_In_ LPVOID    lpvReserved
	)
{
	switch (fdwReason)
	{
	case DLL_QUERY_HMODULE:
		if (lpvReserved != NULL)
			*(HMODULE *)lpvReserved = hAppInstance;
		break;
	case DLL_PROCESS_ATTACH:
		HThisModule = hinstDLL;
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)InitialThread, NULL, NULL, NULL);
		break;
	case DLL_PROCESS_DETACH:
		break;
	}

	return TRUE;
}

int Hwid()
{
	bool allgood;
	CUserCmd* pCmd;
	HANDLE hOlly = FindWindow(TEXT(XorStr("OLLYDBG")), NULL);
	HANDLE hWinDbg = FindWindow(TEXT(XorStr("WinDbgFrameClass")), NULL);
	HANDLE hScylla1 = FindWindow(NULL, TEXT(XorStr("Scylla x86")));
	HANDLE hScylla2 = FindWindow(NULL, TEXT(XorStr("Scylla x64")));
	HANDLE x32_dbg = FindWindow(NULL, TEXT(XorStr("x32_dbg")));
	HANDLE x64_dbg = FindWindow(NULL, TEXT(XorStr("x64_dbg")));
	HANDLE IDA = FindWindow(NULL, TEXT(XorStr("IDA")));
	HANDLE loader = FindWindow(NULL, TEXT(XorStr("KW1337")));

	/*if (IsDebuggerPresent())
	ExitProcess(0);
	if (IsDbgPresentPrefixCheck())
	ExitProcess(0);
	if (Int2DCheck())
	ExitProcess(0);
	if (hOlly)
	ExitProcess(0);
	if (hWinDbg)
	ExitProcess(0);
	if (hScylla1)
	ExitProcess(0);
	if (hScylla2)
	ExitProcess(0);
	if (x32_dbg)
	ExitProcess(0);
	if (x64_dbg)
	ExitProcess(0);
	*/
	std::string darkhwid = XorStr("{846ee340-7039-11de-9d20-806e6f6e6963}");
	std::string Raven = XorStr("{846ee340-7039-11de-9d20-806e6f6e6963}");
	std::string Fach = XorStr("{617c747f-8732-11e6-abaf-806e6f6e6963}");
	std::string UserGUID; // current inject guy hwid

	HW_PROFILE_INFO hwProfileInfo;

	if (GetCurrentHwProfile(&hwProfileInfo) != NULL)
	{
		UserGUID = hwProfileInfo.szHwProfileGuid;
	}

	if (UserGUID == darkhwid || UserGUID == Raven || UserGUID == Fach) //if allowed
	{
		if (loader && UserGUID == Raven || UserGUID == darkhwid || UserGUID == Fach) //cause with dark we dont actually use the loader (when testing it will be annoying to inject by loader everytime), still this is check for the loader if we use it cause it wont actually let us inject
			InitialThread();
		else if (!loader && UserGUID == Raven || UserGUID == darkhwid || UserGUID == Fach) //if not loader so we with dark can inject but still the hwid verification
			InitialThread();
		else if (loader) //and next check for normal ppl xd
			InitialThread();
		else
		{
			char Error[512];
			sprintf(Error, "Loader isnt running, dont close the latest msgbox!");
			MessageBox(0, Error, "ERROR", MB_OK | MB_ICONERROR);
			exit(1);
		}
	}
	else
	{
		//deny
		char ErrorMsg[125];
		sprintf(ErrorMsg, "HWID isnt valid!");
		MessageBox(0, ErrorMsg, "Error, please contact administrator for solution!", MB_OK | MB_ICONERROR);
		exit(1);
	}


	while (DoUnload == false)Sleep(1000);

	Hooks::UndoHooks();
	Sleep(2000); // Make sure none of our hooks are running
	FreeLibraryAndExitThread(HThisModule, 0);

	return 0;
}

