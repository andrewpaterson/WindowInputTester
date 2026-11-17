#include "StandardLib/ClassDefines.h"
#include "StandardLib/Objects.h"
#include "WindowLib/ComponentInput.h"
#include "WindowLib/Window.h"
#include "ScenePage.h"




//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CScenePage::Init(Ptr<CWindow> pWindow, SDialogLook* psLook, Ptr<CFontColours> pFontColours)
{
	PreInit();

	CGridContainer::Init(pWindow);
	SetAlignment(CSH_Stretch, CSV_Stretch);

	mpStatus = OMalloc<CLabel>(pWindow, pFontColours->GetFont(psLook->pcTheme->GetColour(TC_Text)), "No scene read.");
	mpStatus->SetAlignment(CSH_Left, CSV_Center);

	mpView = OMalloc<CSceneView>(pWindow, psLook->pcTheme, &mpStatus, SVB_LeftTurns);
	mpView->AddInputAction(IAF_KeyDown, "Space", this, &CScenePage::SpaceAction);
	mpView->AddInputAction(IAF_KeyDown, "C", this, &CScenePage::CameraAction);

	AddComponent(mpStatus, 0, 0, 1, 1, 1, 0);
	AddComponent(mpView, 0, 1, 1, 1, 1, 1);

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
	M_Pointer(mpView);
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
	return mpView->GetDraw()->ReadScene(szFilename);
}


//////////////////////////////////////////////////////////////////////////
//
// Once the page is in its tab, so the view's mouse, wheel and keys work
// without clicking it first.
//
//////////////////////////////////////////////////////////////////////////
void CScenePage::FocusView(void)
{
	mpView->FocusView();
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CScenePage::SpaceAction(SComponentInputEvent* psEvent)
{
	mpView->GetDraw()->SwitchPlaying();
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
	mpView->GetDraw()->StepView((pcInput == NULL) || !pcInput->IsShift());
}

