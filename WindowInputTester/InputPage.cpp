#include "BaseLib/StringHelper.h"
#include "StandardLib/ClassDefines.h"
#include "WindowLib/Canvas.h"
#include "WindowLib/ClearCanvasDraw.h"
#include "Keyboard.h"
#include "KeyboardCanvasDraw.h"
#include "Mouse.h"
#include "MouseCanvasDraw.h"
#include "InputPage.h"


//////////////////////////////////////////////////////////////////////////
//
// A canvas that stretches to fill its cell.
//
//////////////////////////////////////////////////////////////////////////
static Ptr<CCanvas> CreateInputCanvas(Ptr<CWindow> pWindow, Ptr<CCanvasDraw> pDraw)
{
	Ptr<CCanvas>	pCanvas;

	pCanvas = OMalloc<CCanvas>(pWindow, pDraw);
	pCanvas->SetAlignment(CSH_Stretch, CSV_Stretch);
	return pCanvas;
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CInputPage::Init(Ptr<CWindow> pWindow, Ptr<CWindowTheme> pTheme)
{
	Ptr<CMouse>		pMouse;
	Ptr<CKeyboard>	pKeyboard;
	Ptr<CCanvas>	pMouseCanvas;
	Ptr<CCanvas>	pKeyboardCanvas;

	PreInit();

	CGridContainer::Init(pWindow);
	SetAlignment(CSH_Stretch, CSV_Stretch);
	SetCanGetFocus(true);

	pMouse = OMalloc<CMouse>();
	pMouseCanvas = CreateInputCanvas(pWindow, OMalloc<CMouseCanvasDraw>(pMouse, pTheme));
	AddComponent(pMouseCanvas, 0, 0, 1, 1, 1, 1);
	AddComponent(CreateInputCanvas(pWindow, OMalloc<CClearCanvasDraw>(pTheme->GetColour(TC_Editor))), 1, 0, 1, 1, 1, 1);
	pKeyboard = OMalloc<CKeyboard>();
	pKeyboardCanvas = CreateInputCanvas(pWindow, OMalloc<CKeyboardCanvasDraw>(pKeyboard, pTheme->GetFont(), pTheme));
	AddComponent(pKeyboardCanvas, 0, 1, 2, 1, 1, 1);

	pMouseCanvas->SetDrawnEveryPaint(true);
	pKeyboardCanvas->SetDrawnEveryPaint(true);

	AddInputAction(IAF_AnyKeyDown, NULL, &pMouse, &CMouse::KeyAction);
	AddInputAction(IAF_Movement, NULL, &pMouse, &CMouse::MovementAction);

	PostInit();
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CInputPage::Class(void)
{
	CGridContainer::Class();
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CInputPage::Free(void)
{
	CGridContainer::Free();
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
bool IsInputPage(Ptr<CComponent> pPage)
{
	if (pPage.IsNull())
	{
		return false;
	}
	return StringCompare(pPage->ClassName(), "CInputPage") == 0;
}

