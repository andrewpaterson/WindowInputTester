#include "BaseLib/UTF8.h"
#include "StandardLib/ClassDefines.h"
#include "StandardLib/Objects.h"
#include "SupportLib/ImageDraw.h"
#include "SupportLib/ImageColourConverter.h"
#include "SupportLib/SimpleTextReader.h"
#include "WindowLib/Canvas.h"
#include "WindowLib/Window.h"
#include "KeyboardCanvasDraw.h"


#define KEYBOARD_MARGIN			8		//Pixels between the keyboard and the canvas's sides.
#define KEYBOARD_KEY_GAP		2.0		//Pixels between a key and the edge of its units.
#define KEYBOARD_KEY_RADIUS		0.1		//Of a key's corners, in key units.


//////////////////////////////////////////////////////////////////////////
//
// The font is the colour the text is drawn in.
//
//////////////////////////////////////////////////////////////////////////
void CKeyboardCanvasDraw::Init(Ptr<CKeyboard> pKeyboard, Ptr<CFont> pFont, Ptr<CWindowTheme> pTheme)
{
	PreInit();

	msBackground = pTheme->GetColour(TC_Editor);
	msKeyBorder = pTheme->GetColour(TC_Border);
	msKeyFace = pTheme->GetColour(TC_Panel);
	msKeyLegend = pTheme->GetColour(TC_Text);
	msPressedBorder = pTheme->GetColour(TC_Accent);
	msPressedFace = pTheme->GetColour(TC_ListSelected);

	CCanvasDraw::Init();
	mpCelDraw = OMalloc<CImageCanvasDraw>(Ptr<CImageCel>(), false);
	mpKeyboard = pKeyboard;
	mpFont = pFont;
	mpLayout = OMalloc<CTextLayout>();
	mpGlyphs = OMalloc<CGlyphListFontDraw>();
	msCelSize.Init(-1, -1);
	miCelChanges = 0;

	PostInit();
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CKeyboardCanvasDraw::Class(void)
{
	U_UInt32(msBackground);
	U_UInt32(msKeyBorder);
	U_UInt32(msKeyFace);
	U_UInt32(msKeyLegend);
	U_UInt32(msPressedBorder);
	U_UInt32(msPressedFace);
	CCanvasDraw::Class();
	M_Pointer(mpCelDraw);
	M_Pointer(mpKeyboard);
	M_Pointer(mpFont);
	M_Pointer(mpLayout);
	M_Pointer(mpGlyphs);
	U_2Int32(msCelSize);
	U_Int32(miCelChanges);
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CKeyboardCanvasDraw::Free(void)
{
	CCanvasDraw::Free();
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
bool CKeyboardCanvasDraw::Save(CObjectWriter* pcFile)
{
	return false;
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
bool CKeyboardCanvasDraw::Load(CObjectReader* pcFile)
{
	return false;
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
bool CKeyboardCanvasDraw::Draw(Ptr<CCanvas> pCanvas)
{
	SInt32Vec2			sSize;
	CComponentInput*	pcInput;

	sSize = pCanvas->GetActualSize();
	if ((sSize.x <= 0) || (sSize.y <= 0))
	{
		return true;
	}

	pcInput = pCanvas->GetWindow()->GetComponentInput();
	if (!pCanvas->GetWindow()->GetFocus()->IsActive())
	{
		mpKeyboard->ReleaseAll();
	}
	else if (pcInput)
	{
		mpKeyboard->Update(pcInput);
	}

	if ((sSize.x != msCelSize.x) || (sSize.y != msCelSize.y) || (miCelChanges != mpKeyboard->GetChanges()) || mpCelDraw->GetCel().IsNull())
	{
		MakeCel(pCanvas, sSize);
	}
	return mpCelDraw->Draw(pCanvas);
}


//////////////////////////////////////////////////////////////////////////
//
// The keys are drawn in RGBA, as the anti-aliased shapes need opacity,
// then the image is put in the canvas's colour order and the legends are
// drawn onto it, as the text's glyphs are in the font's order.
//
//////////////////////////////////////////////////////////////////////////
void CKeyboardCanvasDraw::MakeCel(Ptr<CCanvas> pCanvas, SInt32Vec2 sSize)
{
	Ptr<CImage>			pImage;
	CImageDraw			cDraw;
	CImageColourARGB	cBackground;
	CImageColourARGB	cBorder;
	CImageColourARGB	cFace;
	CImageColourARGB	cPressedBorder;
	CImageColourARGB	cPressedFace;
	CImageColourARGB	cLegend;
	SFloat32Vec2		sKeyboardSize;
	float64				fUnit;
	float64				fOriginX;
	float64				fOriginY;
	float64				fRadius;
	size				i;
	size				uiNumKeys;
	Ptr<CKey>			pKey;
	SFloat32Vec2		sPosition;
	SFloat32Vec2		sKeySize;
	float64				fLeft;
	float64				fTop;
	float64				fRight;
	float64				fBottom;

	sKeyboardSize = mpKeyboard->GetSize();
	fUnit = (float64)(sSize.x - 2 * KEYBOARD_MARGIN) / sKeyboardSize.x;
	if ((float64)(sSize.y - 2 * KEYBOARD_MARGIN) / sKeyboardSize.y < fUnit)
	{
		fUnit = (float64)(sSize.y - 2 * KEYBOARD_MARGIN) / sKeyboardSize.y;
	}
	fOriginX = (sSize.x - fUnit * sKeyboardSize.x) / 2;
	fOriginY = (sSize.y - fUnit * sKeyboardSize.y) / 2;
	fRadius = fUnit * KEYBOARD_KEY_RADIUS;

	pImage = OMalloc<CImage>(sSize.x, sSize.y, PT_uint8, IMAGE_DIFFUSE_RED, IMAGE_DIFFUSE_GREEN, IMAGE_DIFFUSE_BLUE, IMAGE_OPACITY, CHANNEL_STOP);
	pImage->Black();

	cBackground.Init(1.0f, Get8BitRedColour(msBackground) / 255.0f, Get8BitGreenColour(msBackground) / 255.0f, Get8BitBlueColour(msBackground) / 255.0f);
	cBorder.Init(1.0f, Get8BitRedColour(msKeyBorder) / 255.0f, Get8BitGreenColour(msKeyBorder) / 255.0f, Get8BitBlueColour(msKeyBorder) / 255.0f);
	cFace.Init(1.0f, Get8BitRedColour(msKeyFace) / 255.0f, Get8BitGreenColour(msKeyFace) / 255.0f, Get8BitBlueColour(msKeyFace) / 255.0f);
	cPressedBorder.Init(1.0f, Get8BitRedColour(msPressedBorder) / 255.0f, Get8BitGreenColour(msPressedBorder) / 255.0f, Get8BitBlueColour(msPressedBorder) / 255.0f);
	cPressedFace.Init(1.0f, Get8BitRedColour(msPressedFace) / 255.0f, Get8BitGreenColour(msPressedFace) / 255.0f, Get8BitBlueColour(msPressedFace) / 255.0f);

	cDraw.Init(&pImage);
	cDraw.SetColour(&cBackground);
	cDraw.DrawRectangle(0, 0, sSize.x, sSize.y);

	uiNumKeys = mpKeyboard->NumKeys();
	for (i = 0; i < uiNumKeys; i++)
	{
		pKey = mpKeyboard->GetKey(i);
		sPosition = pKey->GetPosition();
		sKeySize = pKey->GetSize();

		//The edges are on whole pixels so that only the corners are blended.
		fLeft = (int32)(fOriginX + sPosition.x * fUnit + KEYBOARD_KEY_GAP);
		fTop = (int32)(fOriginY + sPosition.y * fUnit + KEYBOARD_KEY_GAP);
		fRight = (int32)(fOriginX + (sPosition.x + sKeySize.x) * fUnit - KEYBOARD_KEY_GAP);
		fBottom = (int32)(fOriginY + (sPosition.y + sKeySize.y) * fUnit - KEYBOARD_KEY_GAP);

		cDraw.SetColour(pKey->IsPressed() ? &cPressedBorder : &cBorder);
		cDraw.DrawRoundedRectangle(fLeft, fTop, fRight, fBottom, fRadius);
		cDraw.SetColour(pKey->IsPressed() ? &cPressedFace : &cFace);
		cDraw.DrawRoundedRectangle(fLeft + 1, fTop + 1, fRight - 1, fBottom - 1, fRadius - 1);
	}
	cDraw.Kill();
	cPressedFace.Kill();
	cPressedBorder.Kill();
	cFace.Kill();
	cBorder.Kill();
	cBackground.Kill();

	pImage = CImageColourConverter::ConvertColour(pImage, pCanvas->GetColourOrder());

	cLegend.Init(1.0f, Get8BitRedColour(msKeyLegend) / 255.0f, Get8BitGreenColour(msKeyLegend) / 255.0f, Get8BitBlueColour(msKeyLegend) / 255.0f);
	cDraw.Init(&pImage);
	cDraw.SetColour(&cLegend);
	for (i = 0; i < uiNumKeys; i++)
	{
		pKey = mpKeyboard->GetKey(i);
		sPosition = pKey->GetPosition();
		sKeySize = pKey->GetSize();

		//The face is inside the border.
		fLeft = (int32)(fOriginX + sPosition.x * fUnit + KEYBOARD_KEY_GAP) + 1;
		fTop = (int32)(fOriginY + sPosition.y * fUnit + KEYBOARD_KEY_GAP) + 1;
		fRight = (int32)(fOriginX + (sPosition.x + sKeySize.x) * fUnit - KEYBOARD_KEY_GAP) - 1;
		fBottom = (int32)(fOriginY + (sPosition.y + sKeySize.y) * fUnit - KEYBOARD_KEY_GAP) - 1;

		pKey->GetDraw()->Draw(&cDraw, this, fLeft, fTop, fRight, fBottom);
	}
	cDraw.Kill();
	cLegend.Kill();

	mpCelDraw->SetCel(OMalloc<CImageCel>(pImage));
	msCelSize = sSize;
	miCelChanges = mpKeyboard->GetChanges();
}


//////////////////////////////////////////////////////////////////////////
//
// The text centred on the point, its glyphs cut off outside pcClip so a
// legend too wide for its key doesn't spill over the key's border.
//
//////////////////////////////////////////////////////////////////////////
void CKeyboardCanvasDraw::DrawText(CImageDraw* pcDraw, const char* szText, int32 iCentreX, int32 iCentreY, CInt32Rect* pcClip)
{
	CSimpleTextReader		cReader;
	CUTF8					cUTF8;
	Ptr<CText>				pText;
	SInt32Vec2				sTextSize;
	CArrayGlyphPosition*	pasGlyphs;
	SGlyphPosition*			psGlyph;
	size					i;
	size					uiNumGlyphs;
	int32					x;
	int32					y;

	if (szText[0] == '\0')
	{
		return;
	}

	cUTF8.Init((char*)szText);
	cReader.Init(&mpFont);
	pText = cReader.Read(&cUTF8, false);
	cReader.Kill();
	cUTF8.Kill();
	if (pText.IsNull())
	{
		return;
	}

	mpGlyphs->Begin();
	mpLayout->Layout(mpGlyphs, pText);
	mpGlyphs->End();

	sTextSize = mpLayout->GetSize();
	x = iCentreX - sTextSize.x / 2;
	y = iCentreY - sTextSize.y / 2;

	pasGlyphs = mpGlyphs->GetGlyphs();
	uiNumGlyphs = pasGlyphs->NumElements();
	for (i = 0; i < uiNumGlyphs; i++)
	{
		psGlyph = pasGlyphs->Get(i);
		pcDraw->DrawCel(x + psGlyph->sPosition.x, y + psGlyph->sPosition.y, psGlyph->pcGlyph->GetCel(), pcClip);
	}
}
