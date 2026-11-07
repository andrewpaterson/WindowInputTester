#include <windows.h>
#include "BaseLib/GlobalMemory.h"
#include "WinGdiLib/WinGDIWindowFactory.h"
#include "WinInputLib/WinInput.h"
#include "WindowInputTesterApp.h"


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
int PASCAL WinMain(HINSTANCE hInstance,	HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	bool	bResult;

	_CrtSetBreakAlloc(0);
	InitWindowInputTesterGlobals();

	{
		CWinGDIWindowFactory	cNativeFactory;
		CWinInput				cWinInput;

		cNativeFactory.Init(&gcMemoryAllocator, hInstance, hPrevInstance, nCmdShow, "WindowInputTester");

		//The app's input reads the devices from the XML, then adds these.
		cWinInput.Init(NULL, false);
		cNativeFactory.SetRawInput(WinRawInputCallback, cWinInput.GetRaw());

		bResult = RunWindowInputTesterApp(&cNativeFactory, &cWinInput, "../../commonlibraries/Libraries/WinInputLib/xml");

		cNativeFactory.SetRawInput(NULL, NULL);
		cWinInput.Kill();
		cNativeFactory.Kill();
	}

	KillWindowInputTesterGlobals();
	_CrtDumpMemoryLeaks();
	return bResult ? 0 : 1;
}
