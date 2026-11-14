#include "BaseLib/FileUtil.h"
#include "BaseLib/Timer.h"
#include "StandardLib/ClassDefines.h"
#include "StandardLib/Objects.h"
#include "SupportLib/MeshReader.h"
#include "WindowLib/ComponentInput.h"
#include "WindowLib/Window.h"
#include "VRMLPage.h"


#define VRML_ZOOM_FACTOR	1.1f	//KiCad's, for each roll of the wheel.


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CVRMLPage::Init(Ptr<CWindow> pWindow, SDialogLook* psLook, Ptr<CFontColours> pFontColours)
{
	PreInit();

	CGridContainer::Init(pWindow);
	SetAlignment(CSH_Stretch, CSV_Stretch);
	msLastPointer.Init(0, 0);
	mbMiddleDragged = false;

	mpStatus = OMalloc<CLabel>(pWindow, pFontColours->GetFont(psLook->pcTheme->GetColour(TC_Text)), "No VRML file loaded.");
	mpStatus->SetAlignment(CSH_Left, CSV_Center);

	mpDraw = OMalloc<CVRMLCanvasDraw>(psLook->pcTheme);
	mpCanvas = OMalloc<CCanvas>(pWindow, mpDraw);
	mpCanvas->SetAlignment(CSH_Stretch, CSV_Stretch);
	mpCanvas->SetCanGetFocus(true);
	mpCanvas->AddInputAction(IAF_Movement, NULL, this, &CVRMLPage::MovementAction);
	mpCanvas->AddInputAction(IAF_Wheel, NULL, this, &CVRMLPage::WheelAction);
	mpCanvas->AddInputAction(IAF_KeyUp, "Button 3", this, &CVRMLPage::MiddleReleasedAction);

	AddComponent(mpStatus, 0, 0, 1, 1, 1, 0);
	AddComponent(mpCanvas, 0, 1, 1, 1, 1, 1);

	PostInit();
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CVRMLPage::Class(void)
{
	CGridContainer::Class();

	M_Pointer(mpStatus);
	M_Pointer(mpCanvas);
	M_Pointer(mpDraw);
	U_2Int32(msLastPointer);
	U_Bool(mbMiddleDragged);
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CVRMLPage::Free(void)
{
	CGridContainer::Free();
}


//////////////////////////////////////////////////////////////////////////
//
// The CMesh is only needed until the rasteriser has its copy.
//
//////////////////////////////////////////////////////////////////////////
bool CVRMLPage::LoadVRML(const char* szFilename)
{
	Ptr<CMesh>	pMesh;
	CFileUtil	cFileUtil;
	CChars		szName;
	CChars		szStatus;
	int64		iStart;
	bool		bResult;

	szName.Init();
	cFileUtil.GetFileName(&szName, szFilename);
	szStatus.Init(szName.Text());

	iStart = GetDebugTickCount();
	pMesh = OMalloc<CMesh>();
	bResult = ReadMesh(pMesh, szFilename, MT_VRML);
	if (bResult)
	{
		mpDraw->SetMesh(&pMesh);
		szStatus.Append(": ");
		szStatus.Append((int32)mpDraw->NumTriangles());
		szStatus.Append(" triangles, read in ");
		szStatus.Append((int32)(GetDebugTickCount() - iStart));
		szStatus.Append(" ms.");
		mpCanvas->Invalidate();
	}
	else
	{
		szStatus.Append(" could not be read as VRML.");
	}
	mpStatus->SetText(szStatus.Text());

	szStatus.Kill();
	szName.Kill();
	return bResult;
}


//////////////////////////////////////////////////////////////////////////
//
// Once the page is in its tab, so the view's mouse and wheel work without
// clicking it first.
//
//////////////////////////////////////////////////////////////////////////
void CVRMLPage::FocusView(void)
{
	GetWindow()->GetFocus()->SetFocus(mpCanvas);
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CVRMLPage::MovementAction(SComponentInputEvent* psEvent)
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
			mpDraw->Drag(msLastPointer.x, msLastPointer.y, psEvent->x, psEvent->y);
			mpCanvas->Invalidate();
		}
		else if (pcInput->IsSourceDown("Button 2") || pcInput->IsSourceDown("Button 3"))
		{
			mbMiddleDragged = true;
			mpDraw->Pan(iRight, iDown);
			mpCanvas->Invalidate();
		}
	}
	msLastPointer.Init(psEvent->x, psEvent->y);
}


//////////////////////////////////////////////////////////////////////////
//
// Rolling forward is up with Shift, which the window's chords turn into a
// roll to the left, and right with Control.
//
//////////////////////////////////////////////////////////////////////////
void CVRMLPage::WheelAction(SComponentInputEvent* psEvent)
{
	CComponentInput*	pcInput;
	int32				iForward;

	pcInput = GetWindow()->GetComponentInput();
	if (pcInput == NULL)
	{
		return;
	}

	iForward = ((psEvent->iDown < 0) || (psEvent->iRight < 0)) ? 1 : -1;
	if (pcInput->IsShift())
	{
		mpDraw->PanStep(0, -iForward);
	}
	else if (pcInput->IsControl())
	{
		mpDraw->PanStep(iForward, 0);
	}
	else
	{
		mpDraw->Zoom((iForward > 0) ? VRML_ZOOM_FACTOR : (1.0f / VRML_ZOOM_FACTOR));
	}
	mpCanvas->Invalidate();
}


//////////////////////////////////////////////////////////////////////////
//
// A click of the middle button, not the end of its drag.
//
//////////////////////////////////////////////////////////////////////////
void CVRMLPage::MiddleReleasedAction(SComponentInputEvent* psEvent)
{
	psEvent->bPass = true;

	if (!mbMiddleDragged)
	{
		if (mpDraw->LookAtScreen(psEvent->x, psEvent->y))
		{
			mpCanvas->Invalidate();
		}
	}
	mbMiddleDragged = false;
}

