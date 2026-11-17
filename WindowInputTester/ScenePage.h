#ifndef __SCENE_PAGE_H__
#define __SCENE_PAGE_H__
#include "SupportLib/FontColours.h"
#include "WindowLib/Dialog.h"
#include "WindowLib/GridContainer.h"
#include "WindowLib/Label.h"
#include "SceneViewLib/SceneView.h"


//A scene tab's page, which File's Open makes from a scene file: what's drawn and how along the top, over a view of the scene
//(SceneViewLib's CSceneView, drawn by Vulkan or the rasteriser).  The left button's drag turns the view about what it
//looks at, the middle or right button's drag pans, the wheel zooms, R switches between Vulkan and the rasteriser and Home
//puts the view back where its camera started it: those are the view's.  The page adds Space, which pauses and plays the
//animations, and C and Shift+C, which go to the scene's next and last camera, and the whole scene after its last.
class CScenePage : public CGridContainer
{
CONSTRUCTABLE(CScenePage);
DESTRUCTABLE(CScenePage);
protected:
	Ptr<CLabel>			mpStatus;
	Ptr<CSceneView>		mpView;

public:
	void	Init(Ptr<CWindow> pWindow, SDialogLook* psLook, Ptr<CFontColours> pFontColours);
	void	Class(void) override;
	void	Free(void) override;

	bool	ReadScene(const char* szFilename);
	void	FocusView(void);

	void	SpaceAction(SComponentInputEvent* psEvent);
	void	CameraAction(SComponentInputEvent* psEvent);
};


#endif // __SCENE_PAGE_H__

