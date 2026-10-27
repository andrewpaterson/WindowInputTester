#include "StandardLib/ClassDefines.h"
#include "SupportLib/ImageDraw.h"
#include "ShapeKeyDraw.h"


#define SHAPE_KEY_SIZE		0.32	//Of the shape's width and height, as a fraction of the key face's smaller side.
#define SHAPE_KEY_LINE		1.5		//Pixels wide.


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CShapeKeyDraw::Init(EKeyShape eShape)
{
	PreInit();

	CKeyDraw::Init();
	meShape = eShape;

	PostInit();
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CShapeKeyDraw::Class(void)
{
	CKeyDraw::Class();
	U_Enum(meShape);
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CShapeKeyDraw::Free(void)
{
	CKeyDraw::Free();
}


//////////////////////////////////////////////////////////////////////////
//
// The lines are inside the shape's size.  The triangle points up with its
// middle height on the face's middle.
//
//////////////////////////////////////////////////////////////////////////
void CShapeKeyDraw::Draw(CImageDraw* pcDraw, CKeyboardCanvasDraw* pcKeyboardDraw, float64 fLeft, float64 fTop, float64 fRight, float64 fBottom)
{
	float64		x;
	float64		y;
	float64		fHalf;
	float64		fTriangleHalf;

	x = (fLeft + fRight) / 2;
	y = (fTop + fBottom) / 2;
	fHalf = fRight - fLeft;
	if (fBottom - fTop < fHalf)
	{
		fHalf = fBottom - fTop;
	}
	fHalf = (fHalf * SHAPE_KEY_SIZE - SHAPE_KEY_LINE) / 2;

	switch (meShape)
	{
	case KS_Circle:
		pcDraw->DrawRing(x, y, fHalf + SHAPE_KEY_LINE / 2, fHalf - SHAPE_KEY_LINE / 2, 0, 360);
		break;
	case KS_Triangle:
		fTriangleHalf = fHalf * 1.1547;  //An equilateral triangle as tall as the other shapes is 2/sqrt(3) as wide.
		pcDraw->DrawStroke(x, y - fHalf, x + fTriangleHalf, y + fHalf, SHAPE_KEY_LINE);
		pcDraw->DrawStroke(x + fTriangleHalf, y + fHalf, x - fTriangleHalf, y + fHalf, SHAPE_KEY_LINE);
		pcDraw->DrawStroke(x - fTriangleHalf, y + fHalf, x, y - fHalf, SHAPE_KEY_LINE);
		break;
	case KS_Square:
		pcDraw->DrawStroke(x - fHalf, y - fHalf, x + fHalf, y - fHalf, SHAPE_KEY_LINE);
		pcDraw->DrawStroke(x + fHalf, y - fHalf, x + fHalf, y + fHalf, SHAPE_KEY_LINE);
		pcDraw->DrawStroke(x + fHalf, y + fHalf, x - fHalf, y + fHalf, SHAPE_KEY_LINE);
		pcDraw->DrawStroke(x - fHalf, y + fHalf, x - fHalf, y - fHalf, SHAPE_KEY_LINE);
		break;
	case KS_Cross:
		pcDraw->DrawStroke(x - fHalf, y - fHalf, x + fHalf, y + fHalf, SHAPE_KEY_LINE);
		pcDraw->DrawStroke(x + fHalf, y - fHalf, x - fHalf, y + fHalf, SHAPE_KEY_LINE);
		break;
	}
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
EKeyShape CShapeKeyDraw::GetShape(void) { return meShape; }
