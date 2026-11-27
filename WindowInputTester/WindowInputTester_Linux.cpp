#ifdef LINUX_GNU
#include "BaseLib/GlobalMemory.h"
#include "StandardLib/Objects.h"
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
		Ptr<CX11WindowFactory>	pNativeFactory;
		CX11Input				cX11Input;

		//Rooted while the tester runs.  Its windows hold it too, so it's freed after the last of them.
		pNativeFactory = OMalloc<CX11WindowFactory>(&gcMemoryAllocator, (const char*)NULL);
		ORoot()->Add(pNativeFactory);
		bResult = pNativeFactory->IsOpen();
		if (bResult)
		{
			cX11Input.Init();
			pNativeFactory->SetInput(&cX11Input);

			bResult = RunWindowInputTesterApp(&pNativeFactory, &cX11Input, "../../CommonLibraries/Libraries/WinInputLib/xml");

			pNativeFactory->SetInput(NULL);
			cX11Input.Kill();
		}
		ORoot()->Remove(pNativeFactory);
		pNativeFactory = NULL;
	}

	KillWindowInputTesterGlobals();
	return bResult ? 0 : 1;
}


#endif // LINUX_GNU

