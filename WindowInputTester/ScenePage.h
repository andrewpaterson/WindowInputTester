#ifndef __SCENE_PAGE_H__
#define __SCENE_PAGE_H__
#include "SupportLib/FontColours.h"
#include "WindowLib/Canvas.h"
#include "WindowLib/Dialog.h"
#include "WindowLib/GridContainer.h"
#include "WindowLib/Label.h"
#include "SceneCanvasDraw.h"


//A scene tab's page, which File's Open makes from a scene file: what's drawn and how along the top, over a view of the scene
//drawn by Vulkan (see CSceneCanvasDraw).  The left button's drag turns the view about what it looks at, the middle or right
//button's drag pans, and the wheel zooms.  Space pauses and plays the animations, R switches between Vulkan and the
//rasteriser, C and Shift+C go to the scene's next and last camera, and the whole scene after its last, and Home puts the
//view back where its camera started it.
class CScenePage : public CGridContainer
{
CONSTRUCTABLE(CScenePage);
DESTRUCTABLE(CScenePage);
protected:
	Ptr<CLabel>				mpStatus;
	Ptr<CCanvas>			mpCanvas;
	Ptr<CSceneCanvasDraw>	mpDraw;
	SInt32Vec2				msLastPointer;  //In the view, at the last movement over it.

public:
	void	Init(Ptr<CWindow> pWindow, SDialogLook* psLook, Ptr<CFontColours> pFontColours);
	void	Class(void) override;
	void	Free(void) override;

	bool	ReadScene(const char* szFilename);
	void	FocusView(void);

	void	MovementAction(SComponentInputEvent* psEvent);
	void	WheelAction(SComponentInputEvent* psEvent);
	void	SpaceAction(SComponentInputEvent* psEvent);
	void	RendererAction(SComponentInputEvent* psEvent);
	void	HomeAction(SComponentInputEvent* psEvent);
	void	CameraAction(SComponentInputEvent* psEvent);
};


#endif // __SCENE_PAGE_H__

