#ifndef __MOUSE_CANVAS_DRAW_H__
#define __MOUSE_CANVAS_DRAW_H__
#include "WindowLib/ImageCanvasDraw.h"
#include "Mouse.h"
#include "WindowLib/WindowTheme.h"


class CImageDraw;
class CImageColourARGB;
class CWindow;


//Draws a mouse, seen from above, as large as fits in the canvas and centred in it.  Its body is a rounded rectangle
//whose top corners are only a little round and whose bottom corners are almost half its width round, so its bottom
//is nearly a semicircle.  The top of the body is the left and right buttons, split by a line, with the wheel between
//them showing an up arrow over a down arrow.  A pressed button, or the wheel when it is clicked, is highlighted as a
//pressed key is; the arrow of the way the wheel was rolled lights up for a moment.  While the mouse moves an arrow in
//the colours of a pressed key points the way it is moving from the middle of the body below the buttons, longer the
//faster it moves, up to the room there.
//
//The mouse is drawn into a cel the size of the canvas, made again only when the canvas changes size or what the
//mouse is doing changes (as the arrow does all the time the mouse moves), and an image draw puts the cel on the
//canvas.  The buttons are read from the window's component input each time it is drawn; while the window isn't active
//every button is released.
//
//While the arrow fades, or the wheel's arrow is lit, the canvas is drawn again every MOUSE_FRAME_MILLISECONDS with no
//input (the window's PostAfter), so the arrow goes when the mouse stops.  Then nothing is posted and the window waits.
class CMouseCanvasDraw : public CCanvasDraw
{
CONSTRUCTABLE(CMouseCanvasDraw);
DESTRUCTABLE(CMouseCanvasDraw);
protected:
	ARGB32					msBackground;
	ARGB32					msBorder;
	ARGB32					msFace;
	ARGB32					msPressedBorder;
	ARGB32					msPressedFace;
	ARGB32					msArrow;
	ARGB32					msRolledArrow;
	Ptr<CImageCanvasDraw>	mpCelDraw;
	Ptr<CMouse>				mpMouse;
	SInt32Vec2				msCelSize;  //The canvas size the cel was made for, -1, -1 for none yet.
	int32					miCelState;  //The mouse's state when the cel was made.
	SInt32Vec2				msCelArrow;  //The motion arrow the cel was made with, in pixels, 0, 0 for none.
	CCanvas*				mpcCanvas;  //What it last drew, which holds it, and its window: not held.
	CWindow*				mpcWindow;
	bool					mbPosted;  //The next frame is posted to mpcWindow.

public:
	void	Init(Ptr<CMouse> pMouse, Ptr<CWindowTheme> pTheme);
	void	Class(void) override;
	void 	Free(void) override;
	bool	Save(CObjectWriter* pcFile) override;
	bool	Load(CObjectReader* pcFile) override;

	bool	Draw(Ptr<CCanvas> pCanvas) override;
	void	NextFrame(void);

protected:
	void		MakeCel(Ptr<CCanvas> pCanvas, SInt32Vec2 sSize, int32 iState, SInt32Vec2 sArrow);
	bool		GetBody(SInt32Vec2 sSize, float64* pfLeft, float64* pfTop, float64* pfRight, float64* pfBottom);
	void		GetMotionArea(float64 fLeft, float64 fTop, float64 fRight, float64 fBottom, float64* pfCentreX, float64* pfCentreY, float64* pfMaxLength);
	SInt32Vec2	GetArrow(SInt32Vec2 sSize);
	void		DrawMotion(CImageDraw* pcDraw, float64 fCentreX, float64 fCentreY, SInt32Vec2 sArrow, float64 fWidth, CImageColourARGB* pcBorder, CImageColourARGB* pcFace);
	void		DrawButton(CImageDraw* pcDraw, float64 fLeft, float64 fTop, float64 fRight, float64 fBottom, float64 fRadius, bool bLeft, CImageColourARGB* pcBorder, CImageColourARGB* pcFace);
	void		DrawArrow(CImageDraw* pcDraw, float64 fCentreX, float64 fCentreY, float64 fWidth, bool bUp);
};


#endif // __MOUSE_CANVAS_DRAW_H__

