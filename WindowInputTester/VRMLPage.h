#ifndef __VRML_PAGE_H__
#define __VRML_PAGE_H__
#include "BaseLib/Chars.h"
#include "SupportLib/FontColours.h"
#include "WindowLib/Button.h"
#include "WindowLib/Canvas.h"
#include "WindowLib/Dialog.h"
#include "WindowLib/FileChooser.h"
#include "WindowLib/GridContainer.h"
#include "WindowLib/Label.h"
#include "VRMLCanvasDraw.h"


//The VRML tab's page: a Load button and what was loaded along the top, over a view of the mesh.  The view's mouse is
//KiCad's 3D viewer's: the left button's drag orbits a trackball, the middle or right button's drag pans, the wheel
//zooms, Shift and the wheel pans up and down, Control and the wheel pans left and right, and a click of the middle button
//without a drag orbits around the point under the pointer from then on.
//
//Clicking Load only asks for its chooser; the window's tick shows it with ShowDialog.  The mesh is read while the window
//waits.  A file that can't be read leaves the mesh that was showing.
class CVRMLPage : public CGridContainer
{
CONSTRUCTABLE(CVRMLPage);
DESTRUCTABLE(CVRMLPage);
protected:
	SDialogLook*			mpsLook;
	Ptr<CButton>			mpLoad;
	Ptr<CLabel>				mpStatus;
	Ptr<CCanvas>			mpCanvas;
	Ptr<CVRMLCanvasDraw>	mpDraw;
	Ptr<CImageCel>			mpFolderIcon;
	Ptr<CImageCel>			mpFileIcon;
	Ptr<CFileChooser>		mpChooser;  //While Load's chooser is shown.
	CChars					mszLastFile;  //The file last chosen, empty if none has been.
	bool					mbLoadPending;  //Load was clicked; its chooser is shown on the next tick.
	SInt32Vec2				msLastPointer;  //In the view, at the last movement over it.
	bool					mbMiddleDragged;  //The middle button has moved since it was last released.

public:
	void	Init(Ptr<CWindow> pWindow, SDialogLook* psLook, Ptr<CFontColours> pFontColours);
	void	Class(void) override;
	void	Free(void) override;

	void	ShowDialog(void);
	bool	LoadVRML(const char* szFilename);

	void	LoadClicked(void);
	void	MovementAction(SComponentInputEvent* psEvent);
	void	WheelAction(SComponentInputEvent* psEvent);
	void	MiddleReleasedAction(SComponentInputEvent* psEvent);
};


#endif // __VRML_PAGE_H__

