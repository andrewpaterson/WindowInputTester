#include <math.h>
#include "BaseLib/FileUtil.h"
#include "BaseLib/Timer.h"
#include "StandardLib/ClassDefines.h"
#include "StandardLib/Objects.h"
#include "RenderLib/RenderScene.h"
#include "WindowLib/Canvas.h"
#include "WindowLib/Window.h"
#include "SceneCanvasDraw.h"


#define SCENE_FRAME_MILLISECONDS	16		//Between frames while it plays, about 60 a second.
#define SCENE_STATUS_MILLISECONDS	500		//Between changes to the status's frame time while it plays.
#define SCENE_LONGEST_STEP			0.1f	//Seconds the animations move on by at most in a frame, after a pause say.
#define SCENE_FOV					0.9f	//Radians across the width, when the scene has no camera.
#define SCENE_ORBIT_RADIANS			0.01f	//For each pixel dragged.
#define SCENE_PITCH_LIMIT			1.55f	//Radians up or down, short of straight, where the turntable's up would flip.
#define SCENE_RASTER_UNITS			(1 << 20)	//The raster renderer's units the scene's radius is.


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CSceneCanvasDraw::Init(Ptr<CWindowTheme> pTheme, CLabel* pcStatus)
{
	ARGB32	sColour;

	PreInit();

	CCanvasDraw::Init();
	mpCelDraw = OMalloc<CImageCanvasDraw>(Ptr<CImageCel>(), false);
	sColour = pTheme->GetColour(TC_Editor);
	muiBackground = 0xFF000000 | ((uint32)Get8BitRedColour(sColour) << 16) | ((uint32)Get8BitGreenColour(sColour) << 8) | (uint32)Get8BitBlueColour(sColour);
	msSize.Init(-1, -1);

	mszName.Init();
	mapPlayers.Init();
	mpCamera = OMalloc<CCamera>();
	mpCamera->mpConnection = OMalloc<CConnection>();
	mpCamera->mfFOV = SCENE_FOV;

	mcVulkanFactory.Init();
	mcRasteriser.Init();
	memset(&mcTarget, 0, sizeof(mcTarget));
	mbRasterFactory = false;
	mbVulkan = true;

	msCentre.Init(0, 0, 0);
	mfRadius = 1.0f;
	msStartTarget.Init(0, 0, 0);
	mfStartYaw = 0;
	mfStartPitch = 0;
	mfStartDistance = 1.0f;
	msTarget.Init(0, 0, 0);
	mfYaw = 0;
	mfPitch = 0;
	mfDistance = 1.0f;

	mbPlaying = true;
	miLastFrame = 0;
	mfDrawMilliseconds = 0;
	miStatusTime = 0;
	mpcStatus = pcStatus;
	mpcCanvas = NULL;
	mpcWindow = NULL;
	mbPosted = false;

	PostInit();
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CSceneCanvasDraw::Class(void)
{
	CCanvasDraw::Class();
	M_Pointer(mpCelDraw);
	M_Pointer(mpImage);
	U_UInt32(muiBackground);
	U_2Int32(msSize);
	M_Pointer(mpScene);
	U_String(mszName);
	M_Embedded(mapPlayers);
	M_Pointer(mpCamera);
	U_Unknown(CVulkanRenderFactory, mcVulkanFactory);
	M_Pointer(mpVulkanRenderer);
	M_Pointer(mpVulkanFrame);
	U_Unknown(CRasterRenderFactory, mcRasterFactory);
	U_Unknown(CRasteriser, mcRasteriser);
	U_Unknown(CRasterTarget, mcTarget);
	U_Bool(mbRasterFactory);
	M_Pointer(mpRasterFrame);
	U_Bool(mbVulkan);
	U_3Float32(msCentre);
	U_Float32(mfRadius);
	U_3Float32(msStartTarget);
	U_Float32(mfStartYaw);
	U_Float32(mfStartPitch);
	U_Float32(mfStartDistance);
	U_3Float32(msTarget);
	U_Float32(mfYaw);
	U_Float32(mfPitch);
	U_Float32(mfDistance);
	U_Bool(mbPlaying);
	U_Int64(miLastFrame);
	U_Float64(mfDrawMilliseconds);
	U_Int64(miStatusTime);
	U_Pointer(mpcStatus);
	U_Pointer(mpcCanvas);
	U_Pointer(mpcWindow);
	U_Bool(mbPosted);
}


//////////////////////////////////////////////////////////////////////////
//
// The frames go before their renderers, and the meshes, which the render
// scenes hold, before their device.
//
//////////////////////////////////////////////////////////////////////////
void CSceneCanvasDraw::Free(void)
{
	if (mbPosted)
	{
		mpcWindow->CancelPosted(this);
	}
	mpRasterFrame = NULL;
	mpVulkanFrame = NULL;
	mpVulkanRenderer = NULL;
	if (mcTarget.mpiDepths != NULL)
	{
		mcTarget.Kill();
	}
	mcRasteriser.Kill();
	if (mbRasterFactory)
	{
		mcRasterFactory.Kill();
	}
	mcVulkanFactory.Kill();
	mszName.Kill();
	CCanvasDraw::Free();
}


//////////////////////////////////////////////////////////////////////////
//
// A canvas draw isn't saved.
//
//////////////////////////////////////////////////////////////////////////
bool CSceneCanvasDraw::Save(CObjectWriter* pcFile)
{
	return false;
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
bool CSceneCanvasDraw::Load(CObjectReader* pcFile)
{
	return false;
}


//////////////////////////////////////////////////////////////////////////
//
// Reads the scene, starts its animations and sets the view.  False if it
// can't be read, which leaves nothing drawn.
//
//////////////////////////////////////////////////////////////////////////
bool CSceneCanvasDraw::ReadScene(const char* szFilename)
{
	CFileUtil	cFileUtil;

	mpScene = OMalloc<CScene>();
	if (!mpScene->ReadFile(szFilename))
	{
		mpScene = NULL;
		return false;
	}
	mszName.ReInit();
	cFileUtil.GetFileName(&mszName, szFilename);

	MakePlayers();
	Advance(0);
	FindBounds();
	StartView();

	mpVulkanRenderer = mcVulkanFactory.CreateRenderer();
	mbVulkan = mpVulkanRenderer->IsReady();
	if (mbVulkan)
	{
		mpVulkanFrame = OMalloc<CVulkanFrame>(OMalloc<CRenderScene>(mpScene, &mcVulkanFactory, mpVulkanRenderer), mpCamera);
	}
	SetStatus();
	return true;
}


//////////////////////////////////////////////////////////////////////////
//
// A skeleton's animation is played on each instance of the skeleton, and
// the scene's own on the scene.
//
//////////////////////////////////////////////////////////////////////////
void CSceneCanvasDraw::MakePlayers(void)
{
	Ptr<CSceneAnimation>	pAnimation;
	Ptr<CInstance>			pInstance;
	size					i;
	size					j;

	mapPlayers.RemoveAll();
	for (i = 0; i < mpScene->mapAnimations.NumElements(); i++)
	{
		pAnimation = mpScene->mapAnimations.Get(i);
		if (pAnimation->mpSkeleton.IsNull())
		{
			mapPlayers.Add(OMalloc<CAnimationPlayer>(pAnimation, true));
		}
		else
		{
			for (j = 0; j < mpScene->mapInstances.NumElements(); j++)
			{
				pInstance = mpScene->mapInstances.Get(j);
				if (pInstance->mpSkeleton == pAnimation->mpSkeleton)
				{
					mapPlayers.Add(OMalloc<CAnimationPlayer>(pAnimation, pInstance, true));
				}
			}
		}
	}
}


//////////////////////////////////////////////////////////////////////////
//
// Moves the animations on to iNow, by the time since the last frame, or
// none for the first, and places the scene's connections.
//
//////////////////////////////////////////////////////////////////////////
void CSceneCanvasDraw::Advance(int64 iNow)
{
	Ptr<CAnimationPlayer>	pPlayer;
	float32					fSeconds;
	size					i;

	fSeconds = 0;
	if ((miLastFrame != 0) && mbPlaying)
	{
		fSeconds = (float32)(iNow - miLastFrame) / 1000.0f;
		if (fSeconds > SCENE_LONGEST_STEP)
		{
			fSeconds = SCENE_LONGEST_STEP;
		}
	}
	miLastFrame = iNow;

	for (i = 0; i < mapPlayers.NumElements(); i++)
	{
		pPlayer = mapPlayers.Get(i);
		pPlayer->Advance(fSeconds);
		pPlayer->Apply();
	}
	mpScene->UpdateConnections();
}


//////////////////////////////////////////////////////////////////////////
//
// The middle of where the scene's meshes are posed, and how far they go
// from it.
//
//////////////////////////////////////////////////////////////////////////
void CSceneCanvasDraw::FindBounds(void)
{
	CArrayFloat32Vec3	asPositions;
	Ptr<CInstance>		pInstance;
	SFloat32Vec3		sMin;
	SFloat32Vec3		sMax;
	SFloat32Vec3*		psPosition;
	SFloat32Vec3		sOut;
	float32				fRadius;
	bool				bAny;
	size				i;
	size				j;
	size				k;

	bAny = false;
	sMin.Init(0, 0, 0);
	sMax.Init(0, 0, 0);
	asPositions.Init();
	for (i = 0; i < mpScene->mapInstances.NumElements(); i++)
	{
		pInstance = mpScene->mapInstances.Get(i);
		for (j = 0; j < pInstance->mapMeshes.NumElements(); j++)
		{
			if (!pInstance->mapMeshes.Get(j)->Pose(&asPositions))
			{
				continue;
			}
			for (k = 0; k < asPositions.NumElements(); k++)
			{
				psPosition = asPositions.Get(k);
				if (!bAny)
				{
					sMin = *psPosition;
					sMax = *psPosition;
					bAny = true;
				}
				sMin.Init(fminf(sMin.x, psPosition->x), fminf(sMin.y, psPosition->y), fminf(sMin.z, psPosition->z));
				sMax.Init(fmaxf(sMax.x, psPosition->x), fmaxf(sMax.y, psPosition->y), fmaxf(sMax.z, psPosition->z));
			}
		}
	}
	asPositions.Kill();

	msCentre.Init((sMin.x + sMax.x) * 0.5f, (sMin.y + sMax.y) * 0.5f, (sMin.z + sMax.z) * 0.5f);
	sOut.Init(sMax.x - msCentre.x, sMax.y - msCentre.y, sMax.z - msCentre.z);
	fRadius = sqrtf(sOut.x * sOut.x + sOut.y * sOut.y + sOut.z * sOut.z);
	mfRadius = (fRadius > 0) ? fRadius : 1.0f;
}


//////////////////////////////////////////////////////////////////////////
//
// Where the scene's first camera is, looking the way it does about the
// point in front of it nearest the scene's middle, its roll left out as
// the turntable keeps up up.  With no camera, looking along -z at the
// scene's middle from far enough away to see it all across the shorter
// way of a 4 by 3 view.
//
//////////////////////////////////////////////////////////////////////////
void CSceneCanvasDraw::StartView(void)
{
	Ptr<CCamera>	pCamera;
	SFloat4x4*		psWorld;
	SFloat32Vec3	sBack;
	SFloat32Vec3	sToCamera;
	float32			fLength;
	float32			fDistance;
	float32			fHalfFOV;

	pCamera = (mpScene->mapCameras.NumElements() > 0) ? mpScene->mapCameras.Get(0) : Ptr<CCamera>();
	if (pCamera.IsNotNull() && pCamera->mpConnection.IsNotNull())
	{
		mpCamera->mfFOV = pCamera->mfFOV;
		psWorld = &pCamera->mpConnection->msWorldMatrix;
		sBack.Init(psWorld->z.x, psWorld->z.y, psWorld->z.z);
		fLength = sqrtf(sBack.x * sBack.x + sBack.y * sBack.y + sBack.z * sBack.z);
		if (fLength > 0)
		{
			sBack.Init(sBack.x / fLength, sBack.y / fLength, sBack.z / fLength);
			sToCamera.Init(psWorld->pos.x - msCentre.x, psWorld->pos.y - msCentre.y, psWorld->pos.z - msCentre.z);
			fDistance = sToCamera.x * sBack.x + sToCamera.y * sBack.y + sToCamera.z * sBack.z;
			if (fDistance <= 0)
			{
				fDistance = mfRadius;
			}
			mfStartPitch = asinf(fmaxf(-1.0f, fminf(1.0f, sBack.y)));
			mfStartYaw = atan2f(sBack.x, sBack.z);
			mfStartDistance = fDistance;
			msStartTarget.Init(psWorld->pos.x - sBack.x * fDistance, psWorld->pos.y - sBack.y * fDistance, psWorld->pos.z - sBack.z * fDistance);
			ResetView();
			return;
		}
	}

	mpCamera->mfFOV = SCENE_FOV;
	fHalfFOV = atanf(tanf(SCENE_FOV * 0.5f) * 3.0f / 4.0f);
	mfStartYaw = 0;
	mfStartPitch = 0;
	mfStartDistance = mfRadius / sinf(fHalfFOV);
	msStartTarget = msCentre;
	ResetView();
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CSceneCanvasDraw::ResetView(void)
{
	msTarget = msStartTarget;
	mfYaw = mfStartYaw;
	mfPitch = mfStartPitch;
	mfDistance = mfStartDistance;
	Changed();
}


//////////////////////////////////////////////////////////////////////////
//
// The camera's z, which it looks away from, is towards it from the target,
// its x is level and its y is z across x.  Near and far keep the scene in
// front of it.
//
//////////////////////////////////////////////////////////////////////////
void CSceneCanvasDraw::PlaceCamera(void)
{
	SFloat4x4*	psWorld;
	float32		fCosPitch;
	float32		fBackX;
	float32		fBackY;
	float32		fBackZ;
	float32		fSinYaw;
	float32		fCosYaw;

	fCosPitch = cosf(mfPitch);
	fSinYaw = sinf(mfYaw);
	fCosYaw = cosf(mfYaw);
	fBackX = fCosPitch * fSinYaw;
	fBackY = sinf(mfPitch);
	fBackZ = fCosPitch * fCosYaw;

	psWorld = &mpCamera->mpConnection->msWorldMatrix;
	psWorld->x.Init(fCosYaw, 0, -fSinYaw, 0);
	psWorld->y.Init(-fBackY * fSinYaw, fCosPitch, -fBackY * fCosYaw, 0);
	psWorld->z.Init(fBackX, fBackY, fBackZ, 0);
	psWorld->pos.Init(msTarget.x + fBackX * mfDistance, msTarget.y + fBackY * mfDistance, msTarget.z + fBackZ * mfDistance, 1);
	mpCamera->mpConnection->msLocalMatrix = *psWorld;

	mpCamera->mfNear = mfRadius * 0.001f;
	mpCamera->mfFar = (mfDistance + mfRadius) * 4.0f;
}


//////////////////////////////////////////////////////////////////////////
//
// Dragging right turns the scene right, and dragging down tips it towards
// the view.
//
//////////////////////////////////////////////////////////////////////////
void CSceneCanvasDraw::Orbit(int32 iRight, int32 iDown)
{
	mfYaw -= iRight * SCENE_ORBIT_RADIANS;
	mfPitch += iDown * SCENE_ORBIT_RADIANS;
	mfPitch = fmaxf(-SCENE_PITCH_LIMIT, fminf(SCENE_PITCH_LIMIT, mfPitch));
	Changed();
}


//////////////////////////////////////////////////////////////////////////
//
// The scene follows the pointer: a pixel is what a pixel covers at the
// target.
//
//////////////////////////////////////////////////////////////////////////
void CSceneCanvasDraw::Pan(int32 iRight, int32 iDown)
{
	SFloat4x4*	psWorld;
	float32		fPerPixel;

	if (msSize.x <= 0)
	{
		return;
	}
	PlaceCamera();
	psWorld = &mpCamera->mpConnection->msWorldMatrix;
	fPerPixel = 2.0f * mfDistance * tanf(mpCamera->mfFOV * 0.5f) / msSize.x;
	msTarget.x += (-psWorld->x.x * iRight + psWorld->y.x * iDown) * fPerPixel;
	msTarget.y += (-psWorld->x.y * iRight + psWorld->y.y * iDown) * fPerPixel;
	msTarget.z += (-psWorld->x.z * iRight + psWorld->y.z * iDown) * fPerPixel;
	Changed();
}


//////////////////////////////////////////////////////////////////////////
//
// More than 1 is closer.
//
//////////////////////////////////////////////////////////////////////////
void CSceneCanvasDraw::Zoom(float32 fFactor)
{
	mfDistance /= fFactor;
	Changed();
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CSceneCanvasDraw::SwitchPlaying(void)
{
	mbPlaying = !mbPlaying;
	SetStatus();
	Changed();
}


//////////////////////////////////////////////////////////////////////////
//
// The raster renderer's scale makes the scene's radius SCENE_RASTER_UNITS
// of its units.
//
//////////////////////////////////////////////////////////////////////////
void CSceneCanvasDraw::SwitchRenderer(void)
{
	if (mpVulkanRenderer.IsNull() || !mpVulkanRenderer->IsReady())
	{
		return;
	}
	mbVulkan = !mbVulkan;
	mfDrawMilliseconds = 0;
	SetStatus();
	Changed();
}


//////////////////////////////////////////////////////////////////////////
//
// Something drawn has changed: the canvas is drawn again at the next paint.
//
//////////////////////////////////////////////////////////////////////////
void CSceneCanvasDraw::Changed(void)
{
	if (mpcCanvas)
	{
		mpcCanvas->Invalidate();
	}
}


//////////////////////////////////////////////////////////////////////////
//
// Posted by Draw while it plays.
//
//////////////////////////////////////////////////////////////////////////
void CSceneCanvasDraw::NextFrame(void)
{
	mbPosted = false;
	if (GetDebugTickCount() - miStatusTime >= SCENE_STATUS_MILLISECONDS)
	{
		SetStatus();
	}
	Changed();
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CSceneCanvasDraw::SetStatus(void)
{
	CChars	szStatus;

	miStatusTime = GetDebugTickCount();
	if (mpcStatus == NULL)
	{
		return;
	}
	szStatus.Init(mszName.Text());
	szStatus.Append(": ");
	if (mbVulkan)
	{
		szStatus.Append("Vulkan on ");
		szStatus.Append(mpVulkanRenderer->GetDeviceName());
	}
	else if (mpVulkanRenderer.IsNotNull() && mpVulkanRenderer->IsReady())
	{
		szStatus.Append("the raster renderer");
	}
	else
	{
		szStatus.Append("the raster renderer, as there's no device with Vulkan 1.3");
	}
	if (mfDrawMilliseconds > 0)
	{
		szStatus.Append(", ");
		szStatus.Append((int32)(mfDrawMilliseconds + 0.5));
		szStatus.Append(" ms a frame");
	}
	szStatus.Append(mbPlaying ? ", playing." : ", paused.");
	szStatus.Append("  Space pauses, R switches renderer and Home puts the view back.");
	mpcStatus->SetText(szStatus.Text());
	szStatus.Kill();
}


//////////////////////////////////////////////////////////////////////////
//
// The rasteriser's frame and target are made the first time.
//
//////////////////////////////////////////////////////////////////////////
bool CSceneCanvasDraw::DrawRaster(void)
{
	int32	iScale;

	if (mpRasterFrame.IsNull())
	{
		iScale = (int32)(SCENE_RASTER_UNITS / mfRadius);
		mcRasterFactory.Init((iScale > 0) ? iScale : 1);
		mbRasterFactory = true;
		mpRasterFrame = OMalloc<CRasterFrame>(OMalloc<CRenderScene>(mpScene, &mcRasterFactory, mcRasterFactory.CreateRenderer()), mpCamera);
	}
	if (mcTarget.mpiDepths == NULL)
	{
		if (!mcTarget.Init((uint32*)mpImage->GetData(), msSize.x, msSize.y, msSize.x))
		{
			return false;
		}
	}
	mcTarget.Clear(muiBackground);
	mpRasterFrame->Draw(&mcRasteriser, &mcTarget);
	return true;
}


//////////////////////////////////////////////////////////////////////////
//
// B, G, R, X is what both renderers write and what the window's canvas
// holds.
//
//////////////////////////////////////////////////////////////////////////
bool CSceneCanvasDraw::Draw(Ptr<CCanvas> pCanvas)
{
	SInt32Vec2	sSize;
	int64		iNow;
	int64		iStart;
	float64		fMilliseconds;
	bool		bDrawn;

	sSize = pCanvas->GetActualSize();
	if ((sSize.x <= 0) || (sSize.y <= 0) || mpScene.IsNull())
	{
		return true;
	}
	mpcCanvas = &pCanvas;
	mpcWindow = &pCanvas->GetWindow();

	if ((sSize.x != msSize.x) || (sSize.y != msSize.y))
	{
		if (mcTarget.mpiDepths != NULL)
		{
			mcTarget.Kill();
			memset(&mcTarget, 0, sizeof(mcTarget));
		}
		mpImage = OMalloc<CImage>(sSize.x, sSize.y, PT_uint8, IMAGE_DIFFUSE_BLUE, IMAGE_DIFFUSE_GREEN, IMAGE_DIFFUSE_RED, IMAGE_IGNORED, CHANNEL_STOP);
		mpCelDraw->SetCel(OMalloc<CImageCel>(mpImage));
		msSize = sSize;
	}

	iNow = GetDebugTickCount();
	Advance(iNow);
	PlaceCamera();

	iStart = GetDebugTickCount();
	if (mbVulkan)
	{
		bDrawn = mpVulkanFrame->Draw((uint32*)mpImage->GetData(), msSize.x, msSize.y, muiBackground);
	}
	else
	{
		bDrawn = DrawRaster();
	}
	fMilliseconds = (float64)(GetDebugTickCount() - iStart);
	mfDrawMilliseconds = (mfDrawMilliseconds > 0) ? (mfDrawMilliseconds * 0.9 + fMilliseconds * 0.1) : fMilliseconds;

	if (mbPlaying && !mbPosted && (mapPlayers.NumElements() > 0))
	{
		mpcWindow->PostAfter(SCENE_FRAME_MILLISECONDS, this, &CSceneCanvasDraw::NextFrame);
		mbPosted = true;
	}
	if (!bDrawn)
	{
		return false;
	}
	return mpCelDraw->Draw(pCanvas);
}

