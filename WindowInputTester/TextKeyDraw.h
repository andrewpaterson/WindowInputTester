#ifndef __TEXT_KEY_DRAW_H__
#define __TEXT_KEY_DRAW_H__
#include "BaseLib/Chars.h"
#include "KeyDraw.h"


//A legend of text in the middle of the key, in the keyboard draw's font.  Empty text, like the space bar's, draws nothing.
class CTextKeyDraw : public CKeyDraw
{
CONSTRUCTABLE(CTextKeyDraw);
DESTRUCTABLE(CTextKeyDraw);
protected:
	CChars	mszText;

public:
	void	Init(const char* szText);
	void	Class(void) override;
	void	Free(void) override;

	void	Draw(CImageDraw* pcDraw, CKeyboardCanvasDraw* pcKeyboardDraw, float64 fLeft, float64 fTop, float64 fRight, float64 fBottom) override;
	char*	GetText(void);
};


#endif // __TEXT_KEY_DRAW_H__
