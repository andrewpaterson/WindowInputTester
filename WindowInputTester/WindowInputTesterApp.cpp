#include "BaseLib/Logger.h"
#include "StandardLib/Objects.h"
#include "SupportLib/ImageColour.h"
#include "SupportLib/ImageColourConverter.h"
#include "SupportLib/TrueTypeFont.h"
#include "SupportLib/TrueTypeFontFactory.h"
#include "WindowLib/ClearCanvasDraw.h"
#include "WindowLib/GridContainer.h"
#include "WindowLib/Canvas.h"
#include "WindowLib/JewelColours.h"
#include "WindowLib/JewelTheme.h"
#include "WindowLib/JewelImageGenerator.h"
#include "Keyboard.h"
#include "KeyboardCanvasDraw.h"
#include "MouseCanvasDraw.h"
#include "WindowInputTesterApp.h"


#define WINDOW_INPUT_TESTER_EDITOR_TEXT	"The quick brown fox jumps over the lazy dog.\n" \
										"\n" \
										"Type here.  Arrows, Home and End, Page Up and Down move the caret; with Shift they select.\n" \
										"Tab indents, Shift+Tab outdents, Control+D duplicates and Control+X, C and V use the clipboard."

#define WINDOW_INPUT_TESTER_FONT_CHARACTERS	" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~"


//////////////////////////////////////////////////////////////////////////
//
// A canvas that stretches to fill its cell.
//
//////////////////////////////////////////////////////////////////////////
Ptr<CCanvas> CreateCanvas(Ptr<CWindow> pWindow, Ptr<CCanvasDraw> pDraw)
{
	Ptr<CCanvas>	pCanvas;

	pCanvas = OMalloc<CCanvas>(pWindow, pDraw);
	pCanvas->SetAlignment(CSH_Stretch, CSV_Stretch);
	return pCanvas;
}


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
	Ptr<CGridContainer>		pGrid;
	Ptr<CMenuContainer>		pFileMenu;
	Ptr<CTabComponent>		pInputTab;
	Ptr<CKeyboard>			pKeyboard;
	Ptr<CMouse>				pMouse;
	CColourFormatHelper		cFormat;

	mpWindow = OMalloc<CWindow>("Window Input Tester", pcNativeFactory, OMalloc<CWindowInputTesterTick>(&msDialogs), OMalloc<CClearCanvasDraw>(JEWEL_PANEL), CFT_RGBX, CCO_BGR, CRGB_24bit, ARGB_8bit);

	//The font is white and coloured as it is drawn.
	pWhite = GenerateFont("Resources/InterVariable.ttf", "Inter 13px", 13.0f, mpWindow->GetColourOrder());
	mbValid = pWhite.IsNotNull();
	if (mbValid)
	{
		mpFontColours = OMalloc<CFontColours>(pWhite);
		mpTheme = CJewelTheme::CreateDarkTheme(mpWindow, mpFontColours->GetFont(JEWEL_TEXT));
		msDialogLook.pcTheme = &mpTheme;
		msDialogLook.pcDimFont = &mpFontColours->GetFont(JEWEL_ICON);
		msDialogLook.sBackground = JEWEL_PANEL;
		msDialogLook.sDivider = JEWEL_SEPARATOR;
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

		//Two cells on top, each half the width, over one cell spanning both columns.  The rows are each half the height.
		//The page takes focus when clicked so the keys have somewhere to go.
		pGrid = OMalloc<CGridContainer>(mpWindow);
		pGrid->SetAlignment(CSH_Stretch, CSV_Stretch);
		pGrid->SetCanGetFocus(true);
		pMouse = OMalloc<CMouse>();
		pGrid->AddComponent(CreateCanvas(mpWindow, OMalloc<CMouseCanvasDraw>(pMouse)), 0, 0, 1, 1, 1, 1);
		pGrid->AddComponent(CreateCanvas(mpWindow, OMalloc<CClearCanvasDraw>(JEWEL_EDITOR)), 1, 0, 1, 1, 1, 1);
		pKeyboard = OMalloc<CKeyboard>();
		pGrid->AddComponent(CreateCanvas(mpWindow, OMalloc<CKeyboardCanvasDraw>(pKeyboard, mpFontColours->GetFont(JEWEL_TEXT))), 0, 1, 2, 1, 1, 1);

		//The keyboard and the mouse's buttons read the devices as they are drawn.  The wheel's rolls and the mouse's
		//movement are only events, which go from the focussed component (the one under the pointer for movement) up to
		//the main container, which shows them on the mouse and passes them on.
		pMain->AddInputAction(IAF_AnyKeyDown, NULL, &pMouse, &CMouse::KeyAction);
		pMain->AddInputAction(IAF_Movement, NULL, &pMouse, &CMouse::MovementAction);
		mpWindow->GetColourFormat(&cFormat);
		pInputTab = mpTabbed->AddTab("Input", pGrid, CJewelImageGenerator::CreateKeyboard(JEWEL_ICON, &cFormat));
		mpFileMenuListener = OMalloc<CFileMenuListener>(mpWindow, &msDialogLook, mpFontColours, mpTabbed, pFileMenu);
		mpMenuBar->AddListener(&mpFileMenuListener);
		mpFileMenuListener->AddEditorTab("Editor", WINDOW_INPUT_TESTER_EDITOR_TEXT);

		//Double clicking a tab other than Input renames it.
		mpTabNameListener = OMalloc<CTabNameListener>(mpWindow, &msDialogLook, pInputTab);
		mpTabbed->AddListener(&mpTabNameListener);
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

		mpWindow->GetFocus()->SetFocus(pGrid);
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
		mpTabbed->RemoveListener(&mpTabNameListener);
		mpMenuBar->RemoveListener(&mpFileMenuListener);
		mpWindow->SetComponentInput(NULL);
	}
	return mbValid;
}
