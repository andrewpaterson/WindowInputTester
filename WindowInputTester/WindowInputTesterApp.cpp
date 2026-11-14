#include "BaseLib/Logger.h"
#include "BaseLib/TypeNames.h"
#include "BaseLib/TypeConverter.h"
#include "BaseLib/GlobalMemory.h"
#include "BaseLib/GlobalDataTypesIO.h"
#include "StandardLib/Objects.h"
#include "SupportLib/ImageColour.h"
#include "SupportLib/ImageColourConverter.h"
#include "SupportLib/TrueTypeFont.h"
#include "SupportLib/TrueTypeFontFactory.h"
#include "WindowLib/ClearCanvasDraw.h"
#include "WindowLib/GridContainer.h"
#include "WindowLib/Canvas.h"
#include "WindowLib/JewelTheme.h"
#include "WindowLib/Clipboard.h"
#include "WindowInputTesterApp.h"


#define WINDOW_INPUT_TESTER_FONT_CHARACTERS	" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~"


//////////////////////////////////////////////////////////////////////////
//
// Rasterises the glyphs from the TrueType font in white into an image in
// the window's colour order and makes a font from them.
//
//////////////////////////////////////////////////////////////////////////
Ptr<CFont> GenerateFont(const char* szFontFilename, const char* szName, float32 fPixelSize, EColourOrder eOrder)
{
	CTrueTypeFont			cTrueTypeFont;
	CTrueTypeFontFactory	cFactory;
	CImageColourRGB			cColour;
	CChars					szCharacters;
	Ptr<CFontDescription>	pDescription;
	Ptr<CImage>				pImage;
	Ptr<CFont>				pFont;

	cTrueTypeFont.Init();
	if (!cTrueTypeFont.Load((char*)szFontFilename))
	{
		cTrueTypeFont.Kill();
		gcLogger.Error2(__METHOD__, " Could not load font [", szFontFilename, "].", NULL);
		return NULL;
	}

	cColour.Init(1.0f, 1.0f, 1.0f);
	szCharacters.Init(WINDOW_INPUT_TESTER_FONT_CHARACTERS);
	cFactory.Init(&cTrueTypeFont, fPixelSize);
	pDescription = cFactory.Describe((char*)szName, szCharacters.Text());
	pImage = cFactory.Rasterise(pDescription, &cColour);
	if (pImage.IsNotNull())
	{
		if (pImage->GetColourOrder() != eOrder)
		{
			pImage = CImageColourConverter::ConvertColour(pImage, eOrder);
		}
		pFont = OMalloc<CFont>(pDescription, pImage);
	}
	cFactory.Kill();
	szCharacters.Kill();
	cTrueTypeFont.Kill();

	if (pFont.IsNull())
	{
		gcLogger.Error2(__METHOD__, " Could not make a font from [", szFontFilename, "].", NULL);
	}
	return pFont;
}


//////////////////////////////////////////////////////////////////////////
//
// Before it is set up.
//
//////////////////////////////////////////////////////////////////////////
void CWindowInputTesterApp::Init(void)
{
	PreInit();

	memset(&msDialogLook, 0, sizeof(SDialogLook));
	msDialogs.pcFileMenu = NULL;
	msDialogs.pcTabNames = NULL;
	mbValid = false;
	mbInputs = false;

	PostInit();
}


//////////////////////////////////////////////////////////////////////////
//
// The window, its tabs, menu and listeners.  The devices are described by
// the XML in szInputDirectory; pcNativeInput (the platform's keyboard and
// mouse) is added to them.  Run fails if anything here did.
// Call it once the app is in the root, so that what is made here is
// built under the root rather than as an unrooted graph.
//
//////////////////////////////////////////////////////////////////////////
bool CWindowInputTesterApp::Setup(CNativeWindowFactory* pcNativeFactory, CNativeInput* pcNativeInput, const char* szInputDirectory)
{
	Ptr<CFont>				pWhite;
	Ptr<CGridContainer>		pMain;
	Ptr<CMenuContainer>		pFileMenu;
	Ptr<CClearCanvasDraw>	pBackground;

	pBackground = OMalloc<CClearCanvasDraw>(0);  //Coloured once there is a theme.
	mpWindow = OMalloc<CWindow>("Window Input Tester", pcNativeFactory, OMalloc<CWindowInputTesterTick>(&msDialogs), pBackground, CFT_RGBX, CCO_BGR, CRGB_24bit, ARGB_8bit);

	//Only what changed is drawn.  WINDOW_INPUT_TESTER_VERIFY_REDRAW checks every partial paint against drawing all of the
	//window and logs where they differ.
	mpWindow->SetPartialRedraw(true);
	mpWindow->SetVerifyRedraw(getenv("WINDOW_INPUT_TESTER_VERIFY_REDRAW") != NULL);

	//The font is white and coloured as it is drawn.
	pWhite = GenerateFont("Resources/InterVariable.ttf", "Inter 13px", 13.0f, mpWindow->GetColourOrder());
	mbValid = pWhite.IsNotNull();
	if (mbValid)
	{
		mpFontColours = OMalloc<CFontColours>(pWhite);
		mpTheme = CJewelTheme::CreateDarkTheme(mpWindow, mpFontColours);
		pBackground->SetColour(mpTheme->GetColour(TC_Panel));
		msDialogLook.pcTheme = &mpTheme;
		msDialogLook.pcDimFont = &mpFontColours->GetFont(mpTheme->GetColour(TC_Icon));
		mpPasteHistory = OMalloc<CPasteHistoryDialog>(mpWindow, &msDialogLook, mpTheme->GetFont());
		mpWindow->GetClipboard()->GetHistory()->SetChooser(&mpPasteHistory);
		mpTabbed = OMalloc<CTabbedContainer>(mpWindow, mpTheme);
		mpTabbed->SetAlignment(CSH_Stretch, CSV_Stretch);

		//The menu bar along the top over the tabs.
		mpMenuBar = OMalloc<CMenuBar>(mpWindow, mpTheme);
		mpMenuBar->SetAlignment(CSH_Stretch, CSV_Stretch);
		pFileMenu = OMalloc<CMenuContainer>(mpWindow, mpTheme);
		mpMenuBar->AddMenu("File", pFileMenu);
		pMain = OMalloc<CGridContainer>(mpWindow);
		pMain->AddComponent(mpMenuBar, 0, 0, 1, 1, 1, 0);
		pMain->AddComponent(mpTabbed, 0, 1, 1, 1, 1, 1);
		mpWindow->SetContainer(pMain);

		//No tab is open to start with: File's Input Tester, New and Open make them.
		mpFileMenuListener = OMalloc<CFileMenuListener>(mpWindow, &msDialogLook, mpFontColours, mpTabbed, pFileMenu);
		mpMenuBar->GetItemClickEvent()->Add(&mpFileMenuListener, &CFileMenuListener::MenuItemClicked);

		//Double clicking a tab other than Input renames it.
		mpTabNameListener = OMalloc<CTabNameListener>(mpWindow, &msDialogLook, mpTabbed);
		mpTabbed->GetDoubleClickEvent()->Add(&mpTabNameListener, &CTabNameListener::TabDoubleClicked);
		msDialogs.pcFileMenu = &mpFileMenuListener;
		msDialogs.pcTabNames = &mpTabNameListener;

		mpInput = OMalloc<CInput>((CNativeInput*)NULL);
		mbValid = mpInput->Read((char*)szInputDirectory);
		mpInput->AddNativeInput(pcNativeInput);
		mpInputSource = OMalloc<CWindowInputSource>(mpInput);
		mcComponentInput.Init(&mpInputSource, &mpWindow);
		mbInputs = true;
		mcComponentInput.AddKeyboards();
		mcComponentInput.AddMice();
		mpWindow->SetComponentInput(&mcComponentInput);

		mpWindow->SetWindowSize(1280, 800);
	}
	return mbValid;
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CWindowInputTesterApp::Class(void)
{
	M_Pointer(mpWindow);
	M_Pointer(mpFontColours);
	M_Pointer(mpPasteHistory);
	M_Pointer(mpTheme);
	M_Pointer(mpTabbed);
	M_Pointer(mpMenuBar);
	M_Pointer(mpFileMenuListener);
	M_Pointer(mpTabNameListener);
	U_Data(SDialogLook, msDialogLook);
	U_Data(SWindowInputTesterDialogs, msDialogs);
	M_Pointer(mpInput);
	M_Pointer(mpInputSource);
	U_Data(CComponentInput, mcComponentInput);
	U_Bool(mbValid);
	U_Bool(mbInputs);
}


//////////////////////////////////////////////////////////////////////////
//
// Only this object's own members: the window, the listeners and the input
// may already be freed.
//
//////////////////////////////////////////////////////////////////////////
void CWindowInputTesterApp::Free(void)
{
	if (mbInputs)
	{
		mcComponentInput.Kill();
	}
	if (mpPasteHistory.IsNotNull() && mpWindow.IsNotNull())
	{
		mpWindow->GetClipboard()->GetHistory()->SetChooser(NULL);
	}
	msDialogLook.Kill();
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
bool CWindowInputTesterApp::Save(CObjectWriter* pcFile)
{
	return false;
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
bool CWindowInputTesterApp::Load(CObjectReader* pcFile)
{
	return false;
}


//////////////////////////////////////////////////////////////////////////
//
// Runs the window until it is closed (if Init worked), then takes the
// listeners and the component input out of the window.
//
//////////////////////////////////////////////////////////////////////////
bool CWindowInputTesterApp::Run(void)
{
	if (mbValid)
	{
		mpWindow->Show();
	}

	if (mbInputs)
	{
		msDialogs.pcFileMenu = NULL;
		msDialogs.pcTabNames = NULL;
		mpTabbed->GetDoubleClickEvent()->Remove(&mpTabNameListener);
		mpMenuBar->GetItemClickEvent()->Remove(&mpFileMenuListener);
		mpWindow->SetComponentInput(NULL);
	}
	return mbValid;
}


//////////////////////////////////////////////////////////////////////////
//
// What every Codaphela program sets up before it makes objects, and
// takes down after.
//
//////////////////////////////////////////////////////////////////////////
void InitWindowInputTesterGlobals(void)
{
	gcLogger.Init();
	TypesInit();
	TypeConverterInit();
	MemoryInit();
	DataIOInit();
	ObjectsInit();
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void KillWindowInputTesterGlobals(void)
{
	ObjectsFlush();
	ObjectsKill();
	DataIOKill();
	MemoryKill();
	TypeConverterKill();
	TypesKill();
	gcLogger.Kill();
}


//////////////////////////////////////////////////////////////////////////
//
// The app (its window, listeners and input) is held by the root while it
// runs on the host's window factory and input.  szInputDirectory has
// WinInputLib's XML device descriptions.
//
//////////////////////////////////////////////////////////////////////////
bool RunWindowInputTesterApp(CNativeWindowFactory* pcNativeFactory, CNativeInput* pcNativeInput, const char* szInputDirectory)
{
	Ptr<CWindowInputTesterApp>	pApp;
	bool						bResult;

	//Validating every object each time one changes is too slow for the text's glyphs.  They are validated once at the end.
	gcObjects.DisableValidation();

	pApp = OMalloc<CWindowInputTesterApp>();
	ORoot()->Add(pApp);
	bResult = pApp->Setup(pcNativeFactory, pcNativeInput, szInputDirectory) && pApp->Run();
	ORoot()->Remove(pApp);
	pApp = NULL;

	gcObjects.EnableValidation();
	gcObjects.ValidateObjectsConsistency();
	return bResult;
}
