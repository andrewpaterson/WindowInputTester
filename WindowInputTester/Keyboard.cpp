#include "StandardLib/ClassDefines.h"
#include "StandardLib/Objects.h"
#include "TextKeyDraw.h"
#include "Keyboard.h"


#define KEYBOARD_GAP	0.15f	//Between the blocks of keys and below the function row, in key units.


//////////////////////////////////////////////////////////////////////////
//
// Keys are one unit square unless given.  The main keys are 15 units wide,
// the navigation keys 3 and the number pad 4, each a gap from the last.
// The function row is a gap above the rest.  The texts are the keys'
// legends, a key with two legends has its lower one.  The icon keys above
// the navigation keys are named for their icons; the four above the number
// pad are drawn as their shapes.
//
//////////////////////////////////////////////////////////////////////////
void CKeyboard::Init(void)
{
	const char*	aszF1[] = { "F1", "F2", "F3", "F4", NULL };
	const char*	aszF5[] = { "F5", "F6", "F7", "F8", NULL };
	const char*	aszF9[] = { "F9", "F10", "F11", "F12", NULL };
	const char*	aszNumbers[] = { "`", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "=", NULL };
	const char*	aszTop[] = { "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "[", "]", NULL };
	const char*	aszHome[] = { "A", "S", "D", "F", "G", "H", "J", "K", "L", ";", "'", NULL };
	const char*	aszBottom[] = { "Z", "X", "C", "V", "B", "N", "M", ",", ".", "/", NULL };
	const char*	aszIcons[] = { "snap", "mic", "light", NULL };
	const char*	aszNavigationTop[] = { "ins", "home", "pgup", NULL };
	const char*	aszNavigationBottom[] = { "del", "end", "pgdn", NULL };
	const char*	aszArrows[] = { "left", "down", "right", NULL };
	const char*	aszPadTop[] = { "clear", "/", "*", "-", NULL };
	const char*	aszPad7[] = { "7", "8", "9", NULL };
	const char*	aszPad4[] = { "4", "5", "6", NULL };
	const char*	aszPad1[] = { "1", "2", "3", NULL };
	float32		x;
	float32		y;
	float32		fNavigation;
	float32		fPad;

	PreInit();

	maKeys.Init();

	fNavigation = 15 + KEYBOARD_GAP;
	fPad = fNavigation + 3 + KEYBOARD_GAP;
	msSize.Init(fPad + 4, 6 + KEYBOARD_GAP);

	AddKey(0, 0, 1, 1, "esc");
	AddRow(2, 0, aszF1);
	AddRow(6.5f, 0, aszF5);
	AddRow(11, 0, aszF9);
	AddRow(fNavigation, 0, aszIcons);
	AddKey(fPad, 0, KS_Circle);
	AddKey(fPad + 1, 0, KS_Triangle);
	AddKey(fPad + 2, 0, KS_Square);
	AddKey(fPad + 3, 0, KS_Cross);

	y = 1 + KEYBOARD_GAP;
	x = AddRow(0, y, aszNumbers);
	AddKey(x, y, 2, 1, "backspace");
	AddRow(fNavigation, y, aszNavigationTop);
	AddRow(fPad, y, aszPadTop);

	y += 1;
	AddKey(0, y, 1.5f, 1, "tab");
	x = AddRow(1.5f, y, aszTop);
	AddKey(x, y, 1.5f, 1, "\\");
	AddRow(fNavigation, y, aszNavigationBottom);
	x = AddRow(fPad, y, aszPad7);
	AddKey(x, y, 1, 2, "+");

	y += 1;
	AddKey(0, y, 1.75f, 1, "caps lock");
	x = AddRow(1.75f, y, aszHome);
	AddKey(x, y, 2.25f, 1, "enter");
	AddRow(fPad, y, aszPad4);

	y += 1;
	AddKey(0, y, 2.25f, 1, "shift");
	x = AddRow(2.25f, y, aszBottom);
	AddKey(x, y, 2.75f, 1, "shift");
	AddKey(fNavigation + 1, y, 1, 1, "up");
	x = AddRow(fPad, y, aszPad1);
	AddKey(x, y, 1, 2, "enter");

	y += 1;
	AddKey(0, y, 1.25f, 1, "ctrl");
	AddKey(1.25f, y, 1.25f, 1, "option");
	AddKey(2.5f, y, 1.25f, 1, "cmd");
	AddKey(3.75f, y, 6.25f, 1, "");
	AddKey(10, y, 1.25f, 1, "cmd");
	AddKey(11.25f, y, 1.25f, 1, "option");
	AddKey(12.5f, y, 1.25f, 1, "fn");
	AddKey(13.75f, y, 1.25f, 1, "ctrl");
	AddRow(fNavigation, y, aszArrows);
	AddKey(fPad, y, 2, 1, "0");
	AddKey(fPad + 2, y, 1, 1, ".");

	PostInit();
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CKeyboard::Class(void)
{
	M_Embedded(maKeys);
	U_2Float32(msSize);
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CKeyboard::Free(void)
{
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
bool CKeyboard::Save(CObjectWriter* pcFile)
{
	return false;
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
bool CKeyboard::Load(CObjectReader* pcFile)
{
	return false;
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CKeyboard::AddKey(float32 x, float32 y, float32 fWidth, float32 fHeight, const char* szText)
{
	maKeys.Add(OMalloc<CKey>(x, y, fWidth, fHeight, OMalloc<CTextKeyDraw>(szText)));
}


//////////////////////////////////////////////////////////////////////////
//
// A one unit key with a shape for its legend.
//
//////////////////////////////////////////////////////////////////////////
void CKeyboard::AddKey(float32 x, float32 y, EKeyShape eShape)
{
	maKeys.Add(OMalloc<CKey>(x, y, 1.0f, 1.0f, OMalloc<CShapeKeyDraw>(eShape)));
}


//////////////////////////////////////////////////////////////////////////
//
// Adds one unit keys from x along the row until the NULL text.  Returns
// the x after the last key.
//
//////////////////////////////////////////////////////////////////////////
float32 CKeyboard::AddRow(float32 x, float32 y, const char** aszTexts)
{
	size	i;

	for (i = 0; aszTexts[i] != NULL; i++)
	{
		AddKey(x, y, 1, 1, aszTexts[i]);
		x += 1;
	}
	return x;
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
size CKeyboard::NumKeys(void) { return maKeys.Size(); }
Ptr<CKey> CKeyboard::GetKey(size uiIndex) { return maKeys.Get(uiIndex); }
SFloat32Vec2 CKeyboard::GetSize(void) { return msSize; }
