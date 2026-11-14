#include "StandardLib/ClassDefines.h"
#include "StandardLib/Objects.h"
#include "WindowLib/ComponentInput.h"
#include "WindowLib/Window.h"
#include "ScenePage.h"


#define SCENE_ZOOM_FACTOR	1.1f	//For each roll of the wheel.


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CScenePage::Init(Ptr<CWindow> pWindow, SDialogLook* psLook, Ptr<CFontColours> pFontColours)
{
	PreInit();

	CGridContainer::Init(pWindow);
	SetAlignment(CSH_Stretch, CSV_Stretch);
	msLastPointer.Init(0, 0);

	mpStatus = OMalloc<CLabel>(pWindow, pFontColours->GetFont(psLook->pcTheme->GetColour(TC_Text)), "No scene read.");
	mpStatus->SetAlignment(CSH_Left, CSV_Center);

	mpDraw = OMalloc<CSceneCanvasDraw>(psLook->pcTheme, &mpStatus);
	mpCanvas = OMalloc<CCanvas>(pWindow, mpDraw);
	mpCanvas->SetAlignment(CSH_Stretch, CSV_Stretch);
	mpCanvas->SetCanGetFocus(true);
	mpCanvas->AddInputAction(IAF_Movement, NULL, this, &CScenePage::MovementAction);
	mpCanvas->AddInputAction(IAF_Wheel, NULL, this, &CScenePage::WheelAction);
	mpCanvas->AddInputAction(IAF_KeyDown, "Space", this, &CScenePage::SpaceAction);
	mpCanvas->AddInputAction(IAF_KeyDown, "R", this, &CScenePage::RendererAction);
	mpCanvas->AddInputAction(IAF_KeyDown, "Home", this, &CScenePage::HomeAction);
	mpCanvas->AddInputAction(IAF_KeyDown, "C", this, &CScenePage::CameraAction);

	AddComponent(mpStatus, 0, 0, 1, 1, 1, 0);
	AddComponent(mpCanvas, 0, 1, 1, 1, 1, 1);

	PostInit();
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CScenePage::Class(void)
{
	CGridContainer::Class();

	M_Pointer(mpStatus);
	M_Pointer(mpCanvas);
	M_Pointer(mpDraw);
	U_2Int32(msLastPointer);
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CScenePage::Free(void)
{
	CGridContainer::Free();
}


//////////////////////////////////////////////////////////////////////////
//
// False if it can't be read as a scene file.
//
//////////////////////////////////////////////////////////////////////////
bool CScenePage::ReadScene(const char* szFilename)
{
	return mpDraw->ReadScene(szFilename);
}


//////////////////////////////////////////////////////////////////////////
//
// Once the page is in its tab, so the view's mouse, wheel and keys work
// without clicking it first.
//
//////////////////////////////////////////////////////////////////////////
void CScenePage::FocusView(void)
{
	GetWindow()->GetFocus()->SetFocus(mpCanvas);
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CScenePage::MovementAction(SComponentInputEvent* psEvent)
{
	CComponentInput*	pcInput;
	int32				iRight;
	int32				iDown;

	psEvent->bPass = true;

	pcInput = GetWindow()->GetComponentInput();
	if (pcInput == NULL)
	{
		return;
	}

	iRight = psEvent->x - msLastPointer.x;
	iDown = psEvent->y - msLastPointer.y;
	if ((iRight != 0) || (iDown != 0))
	{
		if (pcInput->IsSourceDown("Button 1"))
		{
			mpDraw->Orbit(iRight, iDown);
		}
		else if (pcInput->IsSourceDown("Button 2") || pcInput->IsSourceDown("Button 3"))
		{
			mpDraw->Pan(iRight, iDown);
		}
	}
	msLastPointer.Init(psEvent->x, psEvent->y);
}


//////////////////////////////////////////////////////////////////////////
//
// Rolling forward goes closer.
//
//////////////////////////////////////////////////////////////////////////
void CScenePage::WheelAction(SComponentInputEvent* psEvent)
{
	bool	bForward;

	bForward = (psEvent->iDown < 0) || (psEvent->iRight < 0);
	mpDraw->Zoom(bForward ? SCENE_ZOOM_FACTOR : (1.0f / SCENE_ZOOM_FACTOR));
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CScenePage::SpaceAction(SComponentInputEvent* psEvent)
{
	mpDraw->SwitchPlaying();
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CScenePage::RendererAction(SComponentInputEvent* psEvent)
{
	mpDraw->SwitchRenderer();
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CScenePage::HomeAction(SComponentInputEvent* psEvent)
{
	mpDraw->ResetView();
}


//////////////////////////////////////////////////////////////////////////
//
// Shift+C isn't a chord, so it's C with Shift down.
//
//////////////////////////////////////////////////////////////////////////
void CScenePage::CameraAction(SComponentInputEvent* psEvent)
{
	CComponentInput*	pcInput;

	pcInput = GetWindow()->GetComponentInput();
	mpDraw->StepView((pcInput == NULL) || !pcInput->IsShift());
}

