#include <windows.h>
#include "BaseLib/LeakDetector.h"
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

	BreakOnAllocation(0);
	InitWindowInputTesterGlobals();

	{
		CWinGDIWindowFactory	cNativeFactory;
		CWinInput				cWinInput;

		cNativeFactory.Init(&gcMemoryAllocator, hInstance, hPrevInstance, nCmdShow, "WindowInputTester");

		//The app's input reads the devices from the XML, then adds these.
		cWinInput.Init(NULL, false);
		cNativeFactory.SetRawInput(WinRawInputCallback, cWinInput.GetRaw());

		bResult = RunWindowInputTesterApp(&cNativeFactory, &cWinInput, "../../CommonLibraries/Libraries/WinInputLib/xml");

		cNativeFactory.SetRawInput(NULL, NULL);
		cWinInput.Kill();
		cNativeFactory.Kill();
	}

	KillWindowInputTesterGlobals();
	LeakDetection();
	return bResult ? 0 : 1;
}
