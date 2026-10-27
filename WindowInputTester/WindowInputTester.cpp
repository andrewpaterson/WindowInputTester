#include <windows.h>
#include "BaseLib/Logger.h"
#include "BaseLib/FastFunctions.h"
#include "BaseLib/TypeNames.h"
#include "BaseLib/TypeConverter.h"
#include "BaseLib/GlobalMemory.h"
#include "BaseLib/GlobalDataTypesIO.h"
#include "StandardLib/Objects.h"
#include "SupportLib/ImageColour.h"
#include "SupportLib/ImageColourConverter.h"
#include "SupportLib/TrueTypeFont.h"
#include "SupportLib/TrueTypeFontFactory.h"
#include "SupportLib/FontColours.h"
#include "InputLib/Input.h"
#include "WindowLib/Window.h"
#include "WindowLib/ClearCanvasDraw.h"
#include "WindowLib/GridContainer.h"
#include "WindowLib/Canvas.h"
#include "WindowLib/JewelColours.h"
#include "WindowLib/ComponentInput.h"
#include "WindowLib/JewelTheme.h"
#include "WindowLib/TabbedContainer.h"
#include "WindowLib/FillContainer.h"
#include "WindowLib/ScrollPane.h"
#include "WindowLib/TextArea.h"
#include "WindowLib/JewelButtonImages.h"
#include "WinGdiLib/WinGDIWindowFactory.h"
#include "WinInputLib/WinInput.h"
#include "WindowInputTesterTick.h"
#include "Keyboard.h"
#include "KeyboardCanvasDraw.h"


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
// An editable text area in a scroll pane over the editor's colour.
//
//////////////////////////////////////////////////////////////////////////
Ptr<CFillContainer> CreateEditor(Ptr<CWindow> pWindow, Ptr<CWindowTheme> pTheme, Ptr<CFontColours> pFontColours)
{
	Ptr<CFillContainer>		pEditor;
	Ptr<CScrollPane>		pScroll;
	Ptr<CTextArea>			pTextArea;

	pTextArea = OMalloc<CTextArea>(pWindow, pFontColours,
		"The quick brown fox jumps over the lazy dog.\n"
		"\n"
		"Type here.  Arrows, Home and End, Page Up and Down move the caret; with Shift they select.\n"
		"Tab indents, Shift+Tab outdents, Control+D duplicates and Control+X, C and V use the clipboard.");
	pTextArea->SetColour(JEWEL_TEXT);
	pTextArea->SetMarkColours(pTheme->GetTextSelectionColour(), pTheme->GetCaretColour());
	pTextArea->SetWordWrap(true);
	pTextArea->SetAlignment(CSH_Stretch, CSV_Stretch);

	pScroll = OMalloc<CScrollPane>(pWindow, pTheme);
	pScroll->SetAlignment(CSH_Stretch, CSV_Stretch);
	pScroll->SetPolicy(SBO_Horizontal, SPP_Never);
	pScroll->SetContent(pTextArea);

	pEditor = OMalloc<CFillContainer>(pWindow);
	pEditor->SetAlignment(CSH_Stretch, CSV_Stretch);
	pEditor->AddComponent(CreateCanvas(pWindow, OMalloc<CClearCanvasDraw>(JEWEL_EDITOR)));
	pEditor->AddComponent(pScroll);
	return pEditor;
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
//
//////////////////////////////////////////////////////////////////////////
int PASCAL WinMain(HINSTANCE hInstance,	HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	_CrtSetBreakAlloc(0);

	gcLogger.Init();
	FastFunctionsInit();
	TypesInit();
	TypeConverterInit();
	MemoryInit();
	DataIOInit();
	ObjectsInit();

	bool bResult;

	bResult = true;

	{
		CWinGDIWindowFactory			cNativeFactory;
		Ptr<CWindow>					pWindow;
		Ptr<CWindowInputTesterTick>		pTick;
		Ptr<CGridContainer>				pGrid;
		Ptr<CWindowTheme>				pTheme;
		Ptr<CTabbedContainer>			pTabbed;
		CColourFormatHelper				cFormat;
		Ptr<CFont>						pWhite;
		Ptr<CFontColours>				pFontColours;
		CInput							cInput;
		CWinInput						cWinInput;
		Ptr<CWindowInputSource>			pInputSource;
		CComponentInput					cComponentInput;

		cNativeFactory.Init(&gcMemoryAllocator,
			hInstance,
			hPrevInstance,
			nCmdShow,
			"WindowInputTester");

		//Validating every object each time one changes is too slow for the text's glyphs.  They are validated once at the end.
		gcObjects.DisableValidation();

		//The window is in the root so that changing pointers into its components doesn't make the objects work out again which
		//of them are still on the stack.  Its tick is allocated, not on the stack, because stack objects can't be under the root.
		pTick = OMalloc<CWindowInputTesterTick>();
		pWindow = OMalloc<CWindow>("Window Input Tester", &cNativeFactory, pTick, OMalloc<CClearCanvasDraw>(JEWEL_PANEL), CFT_RGBX, CCO_BGR, CRGB_24bit, ARGB_8bit);
		ORoot()->Add(pWindow);

		//The font is white and coloured as it is drawn.
		pWhite = GenerateFont("Resources/InterVariable.ttf", "Inter 13px", 13.0f, pWindow->GetColourOrder());
		if (pWhite.IsNotNull())
		{
			pFontColours = OMalloc<CFontColours>(pWhite);
			pTheme = CJewelTheme::CreateDarkTheme(pWindow, pFontColours->GetFont(JEWEL_TEXT));
			pTabbed = OMalloc<CTabbedContainer>(pWindow, pTheme);
			pWindow->SetContainer(pTabbed);

			//Two cells on top, each half the width, over one cell spanning both columns.  The rows are each half the height.
			pGrid = OMalloc<CGridContainer>(pWindow);
			pGrid->SetAlignment(CSH_Stretch, CSV_Stretch);
			pGrid->AddComponent(CreateCanvas(pWindow, OMalloc<CClearCanvasDraw>(JEWEL_EDITOR)), 0, 0, 1, 1, 1, 1);
			pGrid->AddComponent(CreateCanvas(pWindow, OMalloc<CClearCanvasDraw>(JEWEL_EDITOR)), 1, 0, 1, 1, 1, 1);
			pGrid->AddComponent(CreateCanvas(pWindow, OMalloc<CKeyboardCanvasDraw>(OMalloc<CKeyboard>(), pFontColours->GetFont(JEWEL_TEXT))), 0, 1, 2, 1, 1, 1);
			pWindow->GetColourFormat(&cFormat);
			pTabbed->AddTab("Input", pGrid, CJewelButtonImages::CreateChevron(JEWEL_BLUE, &cFormat));
			pTabbed->AddTab("Editor", CreateEditor(pWindow, pTheme, pFontColours), NULL, true);

			//The devices are described by the XML so they are added after it is read.
			cInput.Init(NULL);
			cInput.Read("../../CommonLibraries/WinInputLib/xml");
			cWinInput.Init(NULL, false);
			cInput.AddNativeInput(&cWinInput);
			cNativeFactory.SetRawInput(WinRawInputCallback, cWinInput.GetRaw());

			pInputSource = OMalloc<CWindowInputSource>(&cInput);
			cComponentInput.Init(&pInputSource, &pWindow);
			cComponentInput.AddKeyboards();
			cComponentInput.AddMice();
			pWindow->SetComponentInput(&cComponentInput);

			pWindow->SetWindowSize(1280, 800);
			pWindow->Show();

			pWindow->SetComponentInput(NULL);
			cNativeFactory.SetRawInput(NULL, NULL);
			cComponentInput.Kill();
			pInputSource = NULL;
			cInput.Kill();
			cWinInput.Kill();
		}
		else
		{
			bResult = false;
		}

		pGrid = NULL;
		pTabbed = NULL;
		pTheme = NULL;
		pFontColours = NULL;
		pWhite = NULL;
		ORoot()->Remove(pWindow);
		pWindow = NULL;
		pTick = NULL;
		ORoot()->RemoveAll();

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
	FastFunctionsKill();

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
