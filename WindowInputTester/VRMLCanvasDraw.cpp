#include "StandardLib/ClassDefines.h"
#include "StandardLib/Objects.h"
#include "RasterRenderLib/RasterMeshBuild.h"
#include "WindowLib/Canvas.h"
#include "VRMLCanvasDraw.h"


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CVRMLCanvasDraw::Init(Ptr<CWindowTheme> pTheme)
{
	ARGB32	sColour;

	PreInit();

	CCanvasDraw::Init();
	mpCelDraw = OMalloc<CImageCanvasDraw>(Ptr<CImageCel>(), false);
	mpMesh = OMalloc<CRasterMesh>();
	mcCamera.Init();
	mcRasteriser.Init();
	mcTarget.mpiDepths = NULL;
	sColour = pTheme->GetColour(TC_Editor);
	muiBackground = 0xFF000000 | ((uint32)Get8BitRedColour(sColour) << 16) | ((uint32)Get8BitGreenColour(sColour) << 8) | (uint32)Get8BitBlueColour(sColour);
	msSize.Init(-1, -1);
	mbDirty = true;

	PostInit();
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CVRMLCanvasDraw::Class(void)
{
	CCanvasDraw::Class();
	M_Pointer(mpCelDraw);
	M_Pointer(mpImage);
	M_Pointer(mpMesh);
	U_Data(CRasterCamera, mcCamera);
	U_Data(CRasteriser, mcRasteriser);
	U_Data(CRasterTarget, mcTarget);
	U_UInt32(muiBackground);
	U_2Int32(msSize);
	U_Bool(mbDirty);
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CVRMLCanvasDraw::Free(void)
{
	if (mcTarget.mpiDepths != NULL)
	{
		mcTarget.Kill();
	}
	mcRasteriser.Kill();
	CCanvasDraw::Free();
}


//////////////////////////////////////////////////////////////////////////
//
// A canvas draw isn't saved.
//
//////////////////////////////////////////////////////////////////////////
bool CVRMLCanvasDraw::Save(CObjectWriter* pcFile)
{
	return false;
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
bool CVRMLCanvasDraw::Load(CObjectReader* pcFile)
{
	return false;
}


//////////////////////////////////////////////////////////////////////////
//
// B, G, R, X is what the rasteriser writes and what the window's canvas holds.
//
//////////////////////////////////////////////////////////////////////////
bool CVRMLCanvasDraw::Draw(Ptr<CCanvas> pCanvas)
{
	SInt32Vec2	sSize;

	sSize = pCanvas->GetActualSize();
	if ((sSize.x <= 0) || (sSize.y <= 0))
	{
		return true;
	}

	if ((sSize.x != msSize.x) || (sSize.y != msSize.y))
	{
		if (mcTarget.mpiDepths != NULL)
		{
			mcTarget.Kill();
		}
		mpImage = OMalloc<CImage>(sSize.x, sSize.y, PT_uint8, IMAGE_DIFFUSE_BLUE, IMAGE_DIFFUSE_GREEN, IMAGE_DIFFUSE_RED, IMAGE_IGNORED, CHANNEL_STOP);
		mpCelDraw->SetCel(OMalloc<CImageCel>(mpImage));
		if (!mcTarget.Init((uint32*)mpImage->GetData(), sSize.x, sSize.y, sSize.x))
		{
			return false;
		}
		if (msSize.x < 0)
		{
			mcCamera.Fit(sSize.x, sSize.y);
		}
		else
		{
			mcCamera.Resize(sSize.y);
		}
		msSize = sSize;
		mbDirty = true;
	}

	if (mbDirty)
	{
		mcTarget.Clear(muiBackground);
		mcRasteriser.Draw(&mpMesh, &mcCamera, &mcTarget);
		mbDirty = false;
	}
	return mpCelDraw->Draw(pCanvas);
}


//////////////////////////////////////////////////////////////////////////
//
// The rasteriser keeps its own copy of the mesh, so pcMesh needn't last.
// A new mesh is seen from the top, fitted to the canvas.
//
//////////////////////////////////////////////////////////////////////////
void CVRMLCanvasDraw::SetMesh(CMesh* pcMesh)
{
	BuildRasterMesh(&mpMesh, pcMesh);
	mcCamera.Init();
	if (msSize.x > 0)
	{
		mcCamera.Fit(msSize.x, msSize.y);
	}
	mbDirty = true;
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
size CVRMLCanvasDraw::NumTriangles(void)
{
	return mpMesh->masOpaque.NumElements() + mpMesh->masTransparent.NumElements();
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CVRMLCanvasDraw::Drag(int32 iLastX, int32 iLastY, int32 iX, int32 iY)
{
	mcCamera.Drag(iLastX, iLastY, iX, iY, msSize.x, msSize.y);
	mbDirty = true;
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CVRMLCanvasDraw::Pan(int32 iRight, int32 iDown)
{
	mcCamera.Pan(iRight, iDown);
	mbDirty = true;
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CVRMLCanvasDraw::PanStep(int32 iRight, int32 iUp)
{
	mcCamera.PanStep(iRight, iUp);
	mbDirty = true;
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CVRMLCanvasDraw::Zoom(float32 fFactor)
{
	mcCamera.Zoom(fFactor);
	mbDirty = true;
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
bool CVRMLCanvasDraw::LookAtScreen(int32 iX, int32 iY)
{
	if (msSize.x <= 0)
	{
		return false;
	}
	mcCamera.Prepare(msSize.x, msSize.y);
	if (!mcCamera.LookAtScreen(iX, iY, &mpMesh->msMin, &mpMesh->msMax))
	{
		return false;
	}
	mbDirty = true;
	return true;
}

