#include <windows.h>
#include "BaseLib/LeakDetector.h"
#include "BaseLib/GlobalMemory.h"
#include "StandardLib/Objects.h"
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
		Ptr<CWinGDIWindowFactory>	pNativeFactory;
		CWinInput					cWinInput;

		//Rooted while the tester runs.  Its windows hold it too, so it's freed after the last of them.
		pNativeFactory = OMalloc<CWinGDIWindowFactory>(&gcMemoryAllocator, hInstance, hPrevInstance, nCmdShow, "WindowInputTester");
		ORoot()->Add(pNativeFactory);

		//The app's input reads the devices from the XML, then adds these.
		cWinInput.Init(NULL, false);
		pNativeFactory->SetRawInput(WinRawInputCallback, cWinInput.GetRaw());

		bResult = RunWindowInputTesterApp(&pNativeFactory, &cWinInput, "../../CommonLibraries/Libraries/WinInputLib/xml");

		pNativeFactory->SetRawInput(NULL, NULL);
		cWinInput.Kill();
		ORoot()->Remove(pNativeFactory);
		pNativeFactory = NULL;
	}

	KillWindowInputTesterGlobals();
	LeakDetection();
	return bResult ? 0 : 1;
}
