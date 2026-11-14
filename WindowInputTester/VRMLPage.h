#ifndef __VRML_PAGE_H__
#define __VRML_PAGE_H__
#include "BaseLib/Chars.h"
#include "SupportLib/FontColours.h"
#include "WindowLib/Canvas.h"
#include "WindowLib/Dialog.h"
#include "WindowLib/GridContainer.h"
#include "WindowLib/Label.h"
#include "VRMLCanvasDraw.h"


//A VRML tab's page, which File's Open makes: what was loaded along the top, over a view of the mesh.  The view's mouse is
//KiCad's 3D viewer's: the left button's drag orbits a trackball, the middle or right button's drag pans, the wheel
//zooms, Shift and the wheel pans up and down, Control and the wheel pans left and right, and a click of the middle button
//without a drag orbits around the point under the pointer from then on.
//
//The mesh is read while the window waits.  A file that can't be read leaves the mesh that was showing.
class CVRMLPage : public CGridContainer
{
CONSTRUCTABLE(CVRMLPage);
DESTRUCTABLE(CVRMLPage);
protected:
	Ptr<CLabel>				mpStatus;
	Ptr<CCanvas>			mpCanvas;
	Ptr<CVRMLCanvasDraw>	mpDraw;
	SInt32Vec2				msLastPointer;  //In the view, at the last movement over it.
	bool					mbMiddleDragged;  //The middle button has moved since it was last released.

public:
	void	Init(Ptr<CWindow> pWindow, SDialogLook* psLook, Ptr<CFontColours> pFontColours);
	void	Class(void) override;
	void	Free(void) override;

	bool	LoadVRML(const char* szFilename);
	void	FocusView(void);

	void	MovementAction(SComponentInputEvent* psEvent);
	void	WheelAction(SComponentInputEvent* psEvent);
	void	MiddleReleasedAction(SComponentInputEvent* psEvent);
};


#endif // __VRML_PAGE_H__

