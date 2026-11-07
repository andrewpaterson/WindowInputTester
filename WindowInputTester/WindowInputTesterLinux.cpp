#include "BaseLib/GlobalMemory.h"
#include "X11Lib/X11WindowFactory.h"
#include "X11Lib/X11Input.h"
#include "WindowInputTesterApp.h"


//////////////////////////////////////////////////////////////////////////
//
// The window input tester on Linux.  Run from the WindowInputTester
// directory, the resources and WinInputLib's XML are found relative to it.
//
//////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
	bool	bResult;

	InitWindowInputTesterGlobals();

	{
		CX11WindowFactory	cNativeFactory;
		CX11Input			cX11Input;

		bResult = cNativeFactory.Init(&gcMemoryAllocator, NULL);
		if (bResult)
		{
			cX11Input.Init();
			cNativeFactory.SetInput(&cX11Input);

			bResult = RunWindowInputTesterApp(&cNativeFactory, &cX11Input, "../../commonlibraries/Libraries/WinInputLib/xml");

			cNativeFactory.SetInput(NULL);
			cX11Input.Kill();
		}
		cNativeFactory.Kill();
	}

	KillWindowInputTesterGlobals();
	return bResult ? 0 : 1;
}

