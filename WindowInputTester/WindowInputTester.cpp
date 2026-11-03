#include <windows.h>
#include "BaseLib/Logger.h"
#include "BaseLib/TypeNames.h"
#include "BaseLib/TypeConverter.h"
#include "BaseLib/GlobalMemory.h"
#include "BaseLib/GlobalDataTypesIO.h"
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
	_CrtSetBreakAlloc(0);

	gcLogger.Init();
	TypesInit();
	TypeConverterInit();
	MemoryInit();
	DataIOInit();
	ObjectsInit();

	bool bResult;

	bResult = true;

	{
		CWinGDIWindowFactory			cNativeFactory;
		CWinInput						cWinInput;
		Ptr<CWindowInputTesterApp>		pApp;

		cNativeFactory.Init(&gcMemoryAllocator,
			hInstance,
			hPrevInstance,
			nCmdShow,
			"WindowInputTester");

		//Validating every object each time one changes is too slow for the text's glyphs.  They are validated once at the end.
		gcObjects.DisableValidation();

		//The app's input reads the devices from the XML, then adds these.
		cWinInput.Init(NULL, false);
		cNativeFactory.SetRawInput(WinRawInputCallback, cWinInput.GetRaw());

		//The app (its window, listeners and input) is held by the root while it runs.
		pApp = OMalloc<CWindowInputTesterApp>(&cNativeFactory, &cWinInput, "../../CommonLibraries/Libraries/WinInputLib/xml");
		ORoot()->Add(pApp);
		bResult = pApp->Run();
		ORoot()->Remove(pApp);
		pApp = NULL;

		cNativeFactory.SetRawInput(NULL, NULL);
		cWinInput.Kill();

		gcObjects.EnableValidation();
		gcObjects.ValidateObjectsConsistency();

		cNativeFactory.Kill();
	}

	ObjectsFlush();
	ObjectsKill();
	DataIOKill();
	MemoryKill();
	TypeConverterKill();
	TypesKill();

	gcLogger.Kill();

	_CrtDumpMemoryLeaks();

	if (bResult)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}
