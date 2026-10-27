#ifndef __KEYBOARD_CANVAS_DRAW_H__
#define __KEYBOARD_CANVAS_DRAW_H__
#include "SupportLib/ColourARGB32.h"
#include "SupportLib/Int32Rect.h"
#include "SupportLib/Font.h"
#include "SupportLib/TextLayout.h"
#include "SupportLib/GlyphListFontDraw.h"
#include "WindowLib/ImageCanvasDraw.h"
#include "Keyboard.h"


class CImageDraw;

//Draws a keyboard's keys as anti-aliased rounded rectangles with their legends on them, as large as fits in the
//canvas and centred in it, the pressed keys highlighted.  The keyboard is drawn into a cel the size of the canvas,
//made again only when the canvas changes size or keys are pressed or released, and an image draw puts the cel on the
//canvas.  The keys are read from the window's component input each time it is drawn; while the window isn't active
//every key is released.
class CKeyboardCanvasDraw : public CCanvasDraw
{
CONSTRUCTABLE(CKeyboardCanvasDraw);
DESTRUCTABLE(CKeyboardCanvasDraw);
protected:
	Ptr<CImageCanvasDraw>		mpCelDraw;
	Ptr<CKeyboard>				mpKeyboard;
	Ptr<CFont>					mpFont;
	Ptr<CTextLayout>			mpLayout;
	Ptr<CGlyphListFontDraw>		mpGlyphs;
	SInt32Vec2					msCelSize;  //The canvas size the cel was made for, -1, -1 for none yet.
	int32						miCelChanges;  //The keyboard's changes when the cel was made.

public:
	void		Init(Ptr<CKeyboard> pKeyboard, Ptr<CFont> pFont);
	void		Class(void) override;
	void 		Free(void) override;
	bool		Save(CObjectWriter* pcFile) override;
	bool		Load(CObjectReader* pcFile) override;

	bool		Draw(Ptr<CCanvas> pCanvas) override;
	void		DrawText(CImageDraw* pcDraw, const char* szText, int32 iCentreX, int32 iCentreY, CInt32Rect* pcClip);

protected:
	void		MakeCel(Ptr<CCanvas> pCanvas, SInt32Vec2 sSize);
};


#endif // __KEYBOARD_CANVAS_DRAW_H__
