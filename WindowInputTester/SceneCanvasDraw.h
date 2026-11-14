#ifndef __SCENE_CANVAS_DRAW_H__
#define __SCENE_CANVAS_DRAW_H__
#include "BaseLib/Chars.h"
#include "BaseLib/Float32Vec3.h"
#include "RasterLib/Rasteriser.h"
#include "RasterLib/RasterTarget.h"
#include "RasterRenderLib/RasterFrame.h"
#include "RasterRenderLib/RasterRenderFactory.h"
#include "SupportLib/AnimationPlayer.h"
#include "SupportLib/Camera.h"
#include "SupportLib/Image.h"
#include "SupportLib/Scene.h"
#include "VulkanLib/VulkanFrame.h"
#include "VulkanLib/VulkanRenderFactory.h"
#include "WindowLib/CanvasDraw.h"
#include "WindowLib/ImageCanvasDraw.h"
#include "WindowLib/Label.h"
#include "WindowLib/WindowTheme.h"


class CWindow;


//Draws a scene read from a scene file with Vulkan, or with the raster renderer (SwitchRenderer), into a cel the size of the
//canvas, which an image draw puts on the canvas.  Vulkan's frame is read back into the cel, whose bytes are B, G, R, X as the
//window's canvas has them.  With no device that has Vulkan 1.3 it's drawn by the rasteriser, and can't be switched.
//
//Its animations loop: a skeleton's on each instance of the skeleton, and the scene's own on the scene.  While they play the
//canvas is drawn again every SCENE_FRAME_MILLISECONDS (the window's PostAfter).  Paused, or not drawn as its tab isn't
//showing, nothing is posted and the window waits.
//
//The view is the draw's own camera, not the scene's: a turntable, turning about a point by its yaw and pitch, so up stays up.
//It starts where the scene's first camera is, with its field of view, or with none it's framed on the scene posed at the
//start.  StepView goes to the scene's next or last camera, and after the last to the framed view, and round.  ResetView
//puts the view back where its camera, or the framed view, started it.
//
//The status label, which the page holds, says what's drawn and how; it's set outside the paint.
class CSceneCanvasDraw : public CCanvasDraw
{
CONSTRUCTABLE(CSceneCanvasDraw);
DESTRUCTABLE(CSceneCanvasDraw);
protected:
	Ptr<CImageCanvasDraw>		mpCelDraw;
	Ptr<CImage>					mpImage;  //What it's drawn on.
	uint32						muiBackground;  //0xAARRGGBB.
	SInt32Vec2					msSize;  //The canvas size mpImage was made for, -1, -1 for none yet.

	Ptr<CScene>					mpScene;
	CChars						mszName;  //The file's name, for the status.
	CArray<CAnimationPlayer>	mapPlayers;
	Ptr<CCamera>				mpCamera;

	CVulkanRenderFactory		mcVulkanFactory;
	Ptr<CVulkanRenderer>		mpVulkanRenderer;
	Ptr<CVulkanFrame>			mpVulkanFrame;
	CRasterRenderFactory		mcRasterFactory;  //Made when it's first drawn by the rasteriser.
	bool						mbRasterFactory;
	CRasteriser					mcRasteriser;
	CRasterTarget				mcTarget;  //On mpImage, made when it's first drawn by the rasteriser.
	Ptr<CRasterFrame>			mpRasterFrame;  //Made when it's first drawn by the rasteriser.
	bool						mbVulkan;  //Drawn with Vulkan, not the rasteriser.

	SFloat32Vec3				msCentre;  //Of the scene as it was posed at the start,
	float32						mfRadius;  //and how far it goes from there.
	size						muiView;  //The scene camera the view started from, or the number of cameras for the framed view.
	SFloat32Vec3				msStartTarget;  //Where ResetView puts the view.
	float32						mfStartYaw;
	float32						mfStartPitch;
	float32						mfStartDistance;
	SFloat32Vec3				msTarget;  //What the view turns about.
	float32						mfYaw;  //Radians about y, 0 looking along -z.
	float32						mfPitch;  //Radians up from level, the view looking down.
	float32						mfDistance;  //From the target.

	bool						mbPlaying;
	int64						miLastFrame;  //When it was last drawn, 0 if it hasn't been.
	float64						mfDrawMilliseconds;  //Smoothed, what drawing a frame takes.
	int64						miStatusTime;  //When the status was last set.
	CLabel*						mpcStatus;  //The page's: not held.
	CCanvas*					mpcCanvas;  //What it last drew, which holds it, and its window: not held.
	CWindow*					mpcWindow;
	bool						mbPosted;  //The next frame is posted to mpcWindow.

public:
	void		Init(Ptr<CWindowTheme> pTheme, CLabel* pcStatus);
	void		Class(void) override;
	void		Free(void) override;
	bool		Save(CObjectWriter* pcFile) override;
	bool		Load(CObjectReader* pcFile) override;

	bool		ReadScene(const char* szFilename);
	bool		Draw(Ptr<CCanvas> pCanvas) override;
	void		NextFrame(void);

	void		SwitchRenderer(void);
	void		SwitchPlaying(void);
	void		ResetView(void);
	void		StepView(bool bForward);
	void		Orbit(int32 iRight, int32 iDown);
	void		Pan(int32 iRight, int32 iDown);
	void		Zoom(float32 fFactor);

protected:
	void		MakePlayers(void);
	void		Advance(int64 iNow);
	void		FindBounds(void);
	void		StartView(void);
	bool		StartFromCamera(Ptr<CCamera> pCamera);
	void		StartFramed(void);
	void		PlaceCamera(void);
	bool		DrawRaster(void);
	void		SetStatus(void);
	void		Changed(void);
};


#endif // __SCENE_CANVAS_DRAW_H__

