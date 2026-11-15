#include <stdlib.h>
#include "BaseLib/Float4x4.h"
#include "StandardLib/ClassDefines.h"
#include "StandardLib/Objects.h"
#include "SupportLib/Camera.h"
#include "SupportLib/Instance.h"
#include "SupportLib/ShapeSmoothUnion.h"
#include "SupportLib/Sphere.h"
#include "WindowLib/ComponentInput.h"
#include "WindowLib/Window.h"
#include "IsosurfacePage.h"


#define ISOSURFACE_ZOOM_FACTOR		1.1f	//For each roll of the wheel.
#define ISOSURFACE_CAMERA_DISTANCE	10.0f	//From the plane z = 0, where the view starts.


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CIsosurfacePage::Init(Ptr<CWindow> pWindow, SDialogLook* psLook, Ptr<CFontColours> pFontColours)
{
	Ptr<CFont>	pFont;

	PreInit();

	CGridContainer::Init(pWindow);
	SetAlignment(CSH_Stretch, CSV_Stretch);
	msLastPointer.Init(0, 0);
	msPressed.Init(0, 0);
	mbClick = false;
	miSpheres = 0;
	mbInScene = false;

	pFont = pFontColours->GetFont(psLook->pcTheme->GetColour(TC_Text));
	mpStatus = OMalloc<CLabel>(pWindow, pFont, "No spheres.");
	mpStatus->SetAlignment(CSH_Left, CSV_Center);
	mpHelp = OMalloc<CLabel>(pWindow, pFontColours->GetFont(psLook->pcTheme->GetColour(TC_Icon)), "Click to drop a sphere on the plane of the screen.  Drag to turn, middle or right drag to pan, wheel to zoom.  R renderer, Home view, Delete clears.");
	mpHelp->SetAlignment(CSH_Left, CSV_Center);

	mpDraw = OMalloc<CSceneCanvasDraw>(psLook->pcTheme, &mpStatus);
	mpCanvas = OMalloc<CCanvas>(pWindow, mpDraw);
	mpCanvas->SetAlignment(CSH_Stretch, CSV_Stretch);
	mpCanvas->SetCanGetFocus(true);
	mpCanvas->AddInputAction(IAF_Movement, NULL, this, &CIsosurfacePage::MovementAction);
	mpCanvas->AddInputAction(IAF_Wheel, NULL, this, &CIsosurfacePage::WheelAction);
	mpCanvas->AddInputAction(IAF_KeyDown, "Button 1", this, &CIsosurfacePage::PressAction);
	mpCanvas->AddInputAction(IAF_KeyUp, "Button 1", this, &CIsosurfacePage::ReleaseAction);
	mpCanvas->AddInputAction(IAF_KeyDown, "R", this, &CIsosurfacePage::RendererAction);
	mpCanvas->AddInputAction(IAF_KeyDown, "Home", this, &CIsosurfacePage::HomeAction);
	mpCanvas->AddInputAction(IAF_KeyDown, "Delete", this, &CIsosurfacePage::DeleteAction);

	AddComponent(mpStatus, 0, 0, 1, 1, 1, 0);
	AddComponent(mpHelp, 0, 1, 1, 1, 1, 0);
	AddComponent(mpCanvas, 0, 2, 1, 1, 1, 1);

	MakeScene();

	PostInit();
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CIsosurfacePage::Class(void)
{
	CGridContainer::Class();

	M_Pointer(mpStatus);
	M_Pointer(mpHelp);
	M_Pointer(mpCanvas);
	M_Pointer(mpDraw);
	M_Pointer(mpScene);
	M_Pointer(mpEditor);
	M_Pointer(mpShapeEditor);
	M_Pointer(mpShapeNormals);
	M_Pointer(mpShape);
	U_SInt(miSpheres);
	U_Bool(mbInScene);
	U_2Int32(msLastPointer);
	U_2Int32(msPressed);
	U_Bool(mbClick);
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CIsosurfacePage::Free(void)
{
	CGridContainer::Free();
}


//////////////////////////////////////////////////////////////////////////
//
// A camera ten from the origin along z, looking back along -z, so the
// plane of the screen starts as z = 0, and an editor whose stack meshes
// the spheres and gives them the shape's normals, so they're smooth.  The editor's instance goes in with the first sphere: until
// then there's nothing to draw.
//
//////////////////////////////////////////////////////////////////////////
void CIsosurfacePage::MakeScene(void)
{
	Ptr<CCamera>	pCamera;

	mpScene = OMalloc<CScene>();

	pCamera = OMalloc<CCamera>();
	pCamera->mfFOV = 3.14159265f / 3.0f;
	pCamera->mpConnection = OMalloc<CConnection>();
	Float4x4Translation(&pCamera->mpConnection->msWorldMatrix, 0.0f, 0.0f, ISOSURFACE_CAMERA_DISTANCE);
	pCamera->mpConnection->msLocalMatrix = pCamera->mpConnection->msWorldMatrix;
	mpScene->mapCameras.Add(pCamera);

	mpEditor = OMalloc<CMeshEditor>(OMalloc<CMesh>());
	mpShapeEditor = mpEditor->AddModifier<CMeshShapeEditor>(Ptr<CShape>(), ISOSURFACE_SMALLEST_CELL, ISOSURFACE_LARGEST_CELL);
	mpShapeNormals = mpEditor->AddModifier<CMeshShapeNormals>(Ptr<CShape>());

	mpDraw->SetScene(mpScene, "Isosurface: no spheres");
}


//////////////////////////////////////////////////////////////////////////
//
// The editor's instance, placed where its spheres are.
//
//////////////////////////////////////////////////////////////////////////
static Ptr<CInstance> IsosurfaceInstance(Ptr<CMeshEditor> pEditor)
{
	Ptr<CInstance>		pInstance;
	Ptr<CInstanceMesh>	pInstanceMesh;
	Ptr<CConnection>	pConnection;

	pConnection = OMalloc<CConnection>();
	pConnection->msWorldMatrix.Identity();
	pConnection->msLocalMatrix.Identity();
	pInstanceMesh = OMalloc<CInstanceMesh>(pEditor);
	pInstanceMesh->mapConnections.Add(pConnection);
	pInstance = OMalloc<CInstance>();
	pInstance->mapConnections.Add(pConnection);
	pInstance->mapMeshes.Add(pInstanceMesh);
	return pInstance;
}


//////////////////////////////////////////////////////////////////////////
//
// The editor's stack meshes mpShape again (Touch moves its modifier
// counter, which the render scene makes a new mesh for), its instance is
// in the scene while there are spheres, and the view is drawn again.
//
//////////////////////////////////////////////////////////////////////////
void CIsosurfacePage::Remesh(void)
{
	CChars	szName;

	mpShapeEditor->mpShape = mpShape;
	mpShapeNormals->mpShape = mpShape;
	mpEditor->mcModifiers.Touch();
	if ((miSpheres > 0) && !mbInScene)
	{
		mpScene->mapInstances.Add(IsosurfaceInstance(mpEditor));
		mbInScene = true;
	}
	else if ((miSpheres == 0) && mbInScene)
	{
		mpScene->mapInstances.RemoveAll();
		mbInScene = false;
	}

	szName.Init("Isosurface: ");
	if (miSpheres == 0)
	{
		szName.Append("no spheres");
	}
	else
	{
		szName.Append((int32)miSpheres);
		szName.Append((miSpheres == 1) ? " sphere" : " spheres");
	}
	mpDraw->SceneChanged(szName.Text());
	szName.Kill();
}


//////////////////////////////////////////////////////////////////////////
//
// A sphere where the canvas point x, y is on the plane of the screen,
// joined to the others by a smooth union.  False if the point isn't on
// the plane (the view hasn't been drawn yet).
//
//////////////////////////////////////////////////////////////////////////
bool CIsosurfacePage::DropSphere(int32 x, int32 y)
{
	SFloat32Vec3	sCentre;
	Ptr<CSphere>	pSphere;

	if (!mpDraw->ScreenToPlane(x, y, &sCentre))
	{
		return false;
	}

	pSphere = OMalloc<CSphere>(&sCentre, ISOSURFACE_SPHERE_RADIUS);
	if (mpShape.IsNull())
	{
		mpShape = pSphere;
	}
	else
	{
		mpShape = OMalloc<CShapeSmoothUnion>(mpShape, pSphere, ISOSURFACE_BLEND_RADIUS);
	}
	miSpheres++;
	Remesh();
	return true;
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CIsosurfacePage::Clear(void)
{
	mpShape = NULL;
	miSpheres = 0;
	Remesh();
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
int CIsosurfacePage::NumSpheres(void)
{
	return miSpheres;
}


//////////////////////////////////////////////////////////////////////////
//
// Once the page is in its tab, so the view's mouse, wheel and keys work
// without clicking it first.
//
//////////////////////////////////////////////////////////////////////////
void CIsosurfacePage::FocusView(void)
{
	GetWindow()->GetFocus()->SetFocus(mpCanvas);
}


//////////////////////////////////////////////////////////////////////////
//
// The left button turns the view once it's moved further than a click
// may.
//
//////////////////////////////////////////////////////////////////////////
void CIsosurfacePage::MovementAction(SComponentInputEvent* psEvent)
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

	if (mbClick && ((abs(psEvent->x - msPressed.x) > ISOSURFACE_CLICK_PIXELS) || (abs(psEvent->y - msPressed.y) > ISOSURFACE_CLICK_PIXELS)))
	{
		mbClick = false;
	}

	iRight = psEvent->x - msLastPointer.x;
	iDown = psEvent->y - msLastPointer.y;
	if ((iRight != 0) || (iDown != 0))
	{
		if (pcInput->IsSourceDown("Button 1"))
		{
			if (!mbClick)
			{
				mpDraw->Orbit(iRight, iDown);
			}
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
void CIsosurfacePage::WheelAction(SComponentInputEvent* psEvent)
{
	bool	bForward;

	bForward = (psEvent->iDown < 0) || (psEvent->iRight < 0);
	mpDraw->Zoom(bForward ? ISOSURFACE_ZOOM_FACTOR : (1.0f / ISOSURFACE_ZOOM_FACTOR));
}


//////////////////////////////////////////////////////////////////////////
//
// The left button going down may be a click or the start of a drag.
//
//////////////////////////////////////////////////////////////////////////
void CIsosurfacePage::PressAction(SComponentInputEvent* psEvent)
{
	msPressed.Init(psEvent->x, psEvent->y);
	msLastPointer.Init(psEvent->x, psEvent->y);
	mbClick = true;
	psEvent->bPass = true;
}


//////////////////////////////////////////////////////////////////////////
//
// Let go without dragging, it drops a sphere where it went down.
//
//////////////////////////////////////////////////////////////////////////
void CIsosurfacePage::ReleaseAction(SComponentInputEvent* psEvent)
{
	if (mbClick)
	{
		DropSphere(msPressed.x, msPressed.y);
	}
	mbClick = false;
	psEvent->bPass = true;
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CIsosurfacePage::RendererAction(SComponentInputEvent* psEvent)
{
	mpDraw->SwitchRenderer();
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CIsosurfacePage::HomeAction(SComponentInputEvent* psEvent)
{
	mpDraw->ResetView();
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CIsosurfacePage::DeleteAction(SComponentInputEvent* psEvent)
{
	Clear();
}

