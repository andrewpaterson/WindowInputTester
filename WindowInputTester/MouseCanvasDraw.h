#ifndef __MOUSE_CANVAS_DRAW_H__
#define __MOUSE_CANVAS_DRAW_H__
#include "WindowLib/ImageCanvasDraw.h"
#include "Mouse.h"


class CImageDraw;
class CImageColourARGB;


//Draws a mouse, seen from above, as large as fits in the canvas and centred in it.  Its body is a rounded rectangle
//whose top corners are only a little round and whose bottom corners are almost half its width round, so its bottom
//is nearly a semicircle.  The top of the body is the left and right buttons, split by a line, with the wheel between
//them showing an up arrow over a down arrow.  A pressed button, or the wheel when it is clicked, is highlighted as a
//pressed key is; the arrow of the way the wheel was rolled lights up for a moment.
//
//The mouse is drawn into a cel the size of the canvas, made again only when the canvas changes size or what the
//mouse is doing changes, and an image draw puts the cel on the canvas.  While the window isn't active every button is
//released, as their releases aren't heard.
class CMouseCanvasDraw : public CCanvasDraw
{
CONSTRUCTABLE(CMouseCanvasDraw);
DESTRUCTABLE(CMouseCanvasDraw);
protected:
	Ptr<CImageCanvasDraw>	mpCelDraw;
	Ptr<CMouse>				mpMouse;
	SInt32Vec2				msCelSize;  //The canvas size the cel was made for, -1, -1 for none yet.
	int32					miCelState;  //The mouse's state when the cel was made.

public:
	void	Init(Ptr<CMouse> pMouse);
	void	Class(void) override;
	void 	Free(void) override;
	bool	Save(CObjectWriter* pcFile) override;
	bool	Load(CObjectReader* pcFile) override;

	bool	Draw(Ptr<CCanvas> pCanvas) override;

protected:
	void	MakeCel(Ptr<CCanvas> pCanvas, SInt32Vec2 sSize, int32 iState);
	void	DrawButton(CImageDraw* pcDraw, float64 fLeft, float64 fTop, float64 fRight, float64 fBottom, float64 fRadius, bool bLeft, CImageColourARGB* pcBorder, CImageColourARGB* pcFace);
	void	DrawArrow(CImageDraw* pcDraw, float64 fCentreX, float64 fCentreY, float64 fWidth, bool bUp);
};


#endif // __MOUSE_CANVAS_DRAW_H__

