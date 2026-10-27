#include "BaseLib/Timer.h"
#include "StandardLib/ClassDefines.h"
#include "StandardLib/Objects.h"
#include "SupportLib/ImageDraw.h"
#include "SupportLib/VectorPath.h"
#include "SupportLib/ImageColourConverter.h"
#include "WindowLib/JewelColours.h"
#include "WindowLib/Canvas.h"
#include "WindowLib/Window.h"
#include "MouseCanvasDraw.h"


#define MOUSE_MARGIN			16		//Pixels between the mouse and the canvas's sides.
#define MOUSE_ASPECT			0.6		//The mouse's width over its height.
#define MOUSE_TOP_RADIUS		0.15	//Of the top corners, as a part of the width.
#define MOUSE_BOTTOM_RADIUS		0.46	//Of the bottom corners, as a part of the width; a half would be a semicircle.
#define MOUSE_BUTTONS_HEIGHT	0.42	//Of the buttons, as a part of the height.
#define MOUSE_WHEEL_TOP			0.08	//As a part of the height.
#define MOUSE_WHEEL_BOTTOM		0.34	//As a part of the height.
#define MOUSE_WHEEL_WIDTH		0.16	//As a part of the width.
#define MOUSE_ARROW_WIDTH		0.5		//As a part of the wheel's width.
#define MOUSE_ARROW_HEIGHT		0.55	//As a part of the arrow's width.
#define MOUSE_ARROW_CENTRE		0.27	//From the wheel's top to the up arrow's centre, and from its bottom to the down arrow's, as a part of the wheel's height.
#define MOUSE_BACKGROUND		JEWEL_EDITOR
#define MOUSE_BORDER			JEWEL_BORDER
#define MOUSE_FACE				JEWEL_PANEL
#define MOUSE_PRESSED_BORDER	JEWEL_BLUE
#define MOUSE_PRESSED_FACE		JEWEL_SELECTION
#define MOUSE_ARROW				JEWEL_BORDER_HOVER
#define MOUSE_ROLLED_ARROW		JEWEL_BLUE_HOVER


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
static void SetMouseColour(CImageColourARGB* pcColour, ARGB32 sColour)
{
	pcColour->Init(1.0f, Get8BitRedColour(sColour) / 255.0f, Get8BitGreenColour(sColour) / 255.0f, Get8BitBlueColour(sColour) / 255.0f);
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CMouseCanvasDraw::Init(Ptr<CMouse> pMouse)
{
	PreInit();

	CCanvasDraw::Init();
	mpCelDraw = OMalloc<CImageCanvasDraw>(Ptr<CImageCel>(), false);
	mpMouse = pMouse;
	msCelSize.Init(-1, -1);
	miCelState = 0;

	PostInit();
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CMouseCanvasDraw::Class(void)
{
	CCanvasDraw::Class();
	M_Pointer(mpCelDraw);
	M_Pointer(mpMouse);
	U_2Int32(msCelSize);
	U_Int32(miCelState);
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CMouseCanvasDraw::Free(void)
{
	CCanvasDraw::Free();
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
bool CMouseCanvasDraw::Save(CObjectWriter* pcFile)
{
	return false;
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
bool CMouseCanvasDraw::Load(CObjectReader* pcFile)
{
	return false;
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
bool CMouseCanvasDraw::Draw(Ptr<CCanvas> pCanvas)
{
	SInt32Vec2	sSize;
	int32		iState;

	sSize = pCanvas->GetActualSize();
	if ((sSize.x <= 0) || (sSize.y <= 0))
	{
		return true;
	}

	if (!pCanvas->GetWindow()->GetFocus()->IsActive())
	{
		mpMouse->ReleaseAll();
	}

	iState = mpMouse->GetState(GetDebugTickCount());
	if ((sSize.x != msCelSize.x) || (sSize.y != msCelSize.y) || (iState != miCelState) || mpCelDraw->GetCel().IsNull())
	{
		MakeCel(pCanvas, sSize, iState);
	}
	return mpCelDraw->Draw(pCanvas);
}


//////////////////////////////////////////////////////////////////////////
//
// Drawn in RGBA, as the anti-aliased shapes need opacity, then put in the
// canvas's colour order.  Each part is its border colour with its face
// colour a pixel inside it.  The straight edges are on whole pixels so
// that only the curves are blended.  iState is the mouse's MOUSE_PRESSED_
// and MOUSE_ROLLED_ flags.
//
//////////////////////////////////////////////////////////////////////////
void CMouseCanvasDraw::MakeCel(Ptr<CCanvas> pCanvas, SInt32Vec2 sSize, int32 iState)
{
	Ptr<CImage>			pImage;
	CImageDraw			cDraw;
	CImageColourARGB	cBackground;
	CImageColourARGB	cBorder;
	CImageColourARGB	cFace;
	CImageColourARGB	cPressedBorder;
	CImageColourARGB	cPressedFace;
	CImageColourARGB	cArrow;
	CImageColourARGB	cRolledArrow;
	float64				fWidth;
	float64				fHeight;
	float64				fLeft;
	float64				fTop;
	float64				fRight;
	float64				fBottom;
	float64				fTopRadius;
	float64				fBottomRadius;
	float64				fCentre;
	float64				fButtons;
	int32				iWheelWidth;
	float64				fWheelLeft;
	float64				fWheelTop;
	float64				fWheelRight;
	float64				fWheelBottom;
	float64				fWheelRadius;
	float64				fArrowWidth;
	bool				bWheel;

	fHeight = sSize.y - 2 * MOUSE_MARGIN;
	fWidth = fHeight * MOUSE_ASPECT;
	if (fWidth > sSize.x - 2 * MOUSE_MARGIN)
	{
		fWidth = sSize.x - 2 * MOUSE_MARGIN;
		fHeight = fWidth / MOUSE_ASPECT;
	}

	pImage = OMalloc<CImage>(sSize.x, sSize.y, PT_uint8, IMAGE_DIFFUSE_RED, IMAGE_DIFFUSE_GREEN, IMAGE_DIFFUSE_BLUE, IMAGE_OPACITY, CHANNEL_STOP);
	pImage->Black();

	SetMouseColour(&cBackground, MOUSE_BACKGROUND);
	SetMouseColour(&cBorder, MOUSE_BORDER);
	SetMouseColour(&cFace, MOUSE_FACE);
	SetMouseColour(&cPressedBorder, MOUSE_PRESSED_BORDER);
	SetMouseColour(&cPressedFace, MOUSE_PRESSED_FACE);
	SetMouseColour(&cArrow, MOUSE_ARROW);
	SetMouseColour(&cRolledArrow, MOUSE_ROLLED_ARROW);

	cDraw.Init(&pImage);
	cDraw.SetColour(&cBackground);
	cDraw.DrawRectangle(0, 0, sSize.x, sSize.y);

	if ((fWidth > 16) && (fHeight > 16))
	{
		fLeft = (int32)((sSize.x - fWidth) / 2);
		fTop = (int32)((sSize.y - fHeight) / 2);
		fRight = (int32)(fLeft + fWidth);
		fBottom = (int32)(fTop + fHeight);
		fWidth = fRight - fLeft;
		fHeight = fBottom - fTop;
		fTopRadius = fWidth * MOUSE_TOP_RADIUS;
		fBottomRadius = fWidth * MOUSE_BOTTOM_RADIUS;

		cDraw.SetColour(&cBorder);
		cDraw.DrawRoundedRectangle(fLeft, fTop, fRight, fBottom, fTopRadius, fBottomRadius);
		cDraw.SetColour(&cFace);
		cDraw.DrawRoundedRectangle(fLeft + 1, fTop + 1, fRight - 1, fBottom - 1, fTopRadius - 1, fBottomRadius - 1);

		//The buttons' bottom edge and the line between them, then the buttons pressed over them.
		fCentre = (int32)((fLeft + fRight) / 2);
		fButtons = (int32)(fTop + fHeight * MOUSE_BUTTONS_HEIGHT);
		cDraw.SetColour(&cBorder);
		cDraw.DrawRectangle(fLeft + 1, fButtons, fRight - 1, fButtons + 1);
		cDraw.DrawRectangle(fCentre, fTop + 1, fCentre + 1, fButtons);
		if (iState & MOUSE_PRESSED_LEFT)
		{
			DrawButton(&cDraw, fLeft, fTop, fCentre + 1, fButtons + 1, fTopRadius, true, &cPressedBorder, &cPressedFace);
		}
		if (iState & MOUSE_PRESSED_RIGHT)
		{
			DrawButton(&cDraw, fCentre, fTop, fRight, fButtons + 1, fTopRadius, false, &cPressedBorder, &cPressedFace);
		}

		//The wheel, round ended and an odd number of pixels wide, centred on the line between the buttons.
		bWheel = (iState & MOUSE_PRESSED_WHEEL) != 0;
		iWheelWidth = ((int32)(fWidth * MOUSE_WHEEL_WIDTH)) | 1;
		fWheelLeft = fCentre - iWheelWidth / 2;
		fWheelRight = fWheelLeft + iWheelWidth;
		fWheelTop = (int32)(fTop + fHeight * MOUSE_WHEEL_TOP);
		fWheelBottom = (int32)(fTop + fHeight * MOUSE_WHEEL_BOTTOM);
		fWheelRadius = iWheelWidth / 2.0;
		cDraw.SetColour(bWheel ? &cPressedBorder : &cBorder);
		cDraw.DrawRoundedRectangle(fWheelLeft, fWheelTop, fWheelRight, fWheelBottom, fWheelRadius);
		cDraw.SetColour(bWheel ? &cPressedFace : &cFace);
		cDraw.DrawRoundedRectangle(fWheelLeft + 1, fWheelTop + 1, fWheelRight - 1, fWheelBottom - 1, fWheelRadius - 1);

		//Which way the wheel rolls.
		fArrowWidth = iWheelWidth * MOUSE_ARROW_WIDTH;
		cDraw.SetColour((iState & MOUSE_ROLLED_UP) ? &cRolledArrow : &cArrow);
		DrawArrow(&cDraw, fCentre + 0.5, fWheelTop + (fWheelBottom - fWheelTop) * MOUSE_ARROW_CENTRE, fArrowWidth, true);
		cDraw.SetColour((iState & MOUSE_ROLLED_DOWN) ? &cRolledArrow : &cArrow);
		DrawArrow(&cDraw, fCentre + 0.5, fWheelBottom - (fWheelBottom - fWheelTop) * MOUSE_ARROW_CENTRE, fArrowWidth, false);
	}

	cDraw.Kill();
	cRolledArrow.Kill();
	cArrow.Kill();
	cPressedFace.Kill();
	cPressedBorder.Kill();
	cFace.Kill();
	cBorder.Kill();
	cBackground.Kill();

	pImage = CImageColourConverter::ConvertColour(pImage, pCanvas->GetColourOrder());
	mpCelDraw->SetCel(OMalloc<CImageCel>(pImage));
	msCelSize = sSize;
	miCelState = iState;
}


//////////////////////////////////////////////////////////////////////////
//
// A pressed button: the rectangle in the border colour with the face a
// pixel inside it.  Only its outer top corner (the left one for the left
// button) is round, fRadius as the body's.
//
//////////////////////////////////////////////////////////////////////////
void CMouseCanvasDraw::DrawButton(CImageDraw* pcDraw, float64 fLeft, float64 fTop, float64 fRight, float64 fBottom, float64 fRadius, bool bLeft, CImageColourARGB* pcBorder, CImageColourARGB* pcFace)
{
	CVectorPath		cPath;
	SFloat64Vec2	sPoint;
	SFloat64Vec2	sCenter;
	int				iInset;
	float64			l;
	float64			t;
	float64			r;
	float64			b;
	float64			fRound;

	for (iInset = 0; iInset <= 1; iInset++)
	{
		l = fLeft + iInset;
		t = fTop + iInset;
		r = fRight - iInset;
		b = fBottom - iInset;
		fRound = fRadius - iInset;
		if (fRound < 0)
		{
			fRound = 0;
		}

		cPath.Init();
		if (bLeft)
		{
			sPoint.Init(l + fRound, t);
			cPath.MoveTo(&sPoint);
			sPoint.Init(r, t);
			cPath.LineTo(&sPoint);
			sPoint.Init(r, b);
			cPath.LineTo(&sPoint);
			sPoint.Init(l, b);
			cPath.LineTo(&sPoint);
			sPoint.Init(l, t + fRound);
			cPath.LineTo(&sPoint);
			if (fRound > 0)
			{
				sCenter.Init(l + fRound, t + fRound);
				sPoint.Init(l + fRound, t);
				cPath.ArcTo(&sPoint, &sCenter, false);
			}
		}
		else
		{
			sPoint.Init(l, t);
			cPath.MoveTo(&sPoint);
			sPoint.Init(r - fRound, t);
			cPath.LineTo(&sPoint);
			if (fRound > 0)
			{
				sCenter.Init(r - fRound, t + fRound);
				sPoint.Init(r, t + fRound);
				cPath.ArcTo(&sPoint, &sCenter, false);
			}
			sPoint.Init(r, b);
			cPath.LineTo(&sPoint);
			sPoint.Init(l, b);
			cPath.LineTo(&sPoint);
			sPoint.Init(l, t);
			cPath.LineTo(&sPoint);
		}

		pcDraw->SetColour(iInset == 0 ? pcBorder : pcFace);
		pcDraw->DrawPath(&cPath);
		cPath.Kill();
	}
}


//////////////////////////////////////////////////////////////////////////
//
// A filled triangle fWidth across, pointing up or down, centred on the
// point, in the draw's colour.
//
//////////////////////////////////////////////////////////////////////////
void CMouseCanvasDraw::DrawArrow(CImageDraw* pcDraw, float64 fCentreX, float64 fCentreY, float64 fWidth, bool bUp)
{
	SFloat64Vec2	asPoints[3];
	float64			fHalfHeight;
	float64			fTip;
	float64			fBase;

	fHalfHeight = fWidth * MOUSE_ARROW_HEIGHT / 2;
	fTip = bUp ? fCentreY - fHalfHeight : fCentreY + fHalfHeight;
	fBase = bUp ? fCentreY + fHalfHeight : fCentreY - fHalfHeight;
	asPoints[0].Init(fCentreX, fTip);
	asPoints[1].Init(fCentreX + fWidth / 2, fBase);
	asPoints[2].Init(fCentreX - fWidth / 2, fBase);
	pcDraw->DrawPolygon(asPoints, 3);
}

