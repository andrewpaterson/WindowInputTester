#ifndef __SHAPE_KEY_DRAW_H__
#define __SHAPE_KEY_DRAW_H__
#include "KeyDraw.h"


enum EKeyShape
{
	KS_Circle,
	KS_Triangle,
	KS_Square,
	KS_Cross,
};


//A legend of an outlined shape in the middle of the key, anti-aliased.
class CShapeKeyDraw : public CKeyDraw
{
CONSTRUCTABLE(CShapeKeyDraw);
DESTRUCTABLE(CShapeKeyDraw);
protected:
	EKeyShape	meShape;

public:
	void		Init(EKeyShape eShape);
	void		Class(void) override;
	void		Free(void) override;

	void		Draw(CImageDraw* pcDraw, CKeyboardCanvasDraw* pcKeyboardDraw, float64 fLeft, float64 fTop, float64 fRight, float64 fBottom) override;
	EKeyShape	GetShape(void);
};


#endif // __SHAPE_KEY_DRAW_H__
