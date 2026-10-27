#include "BaseLib/StringHelper.h"
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
// the navigation keys are named for their icons and are the keys a PC
// keyboard has there; the four above the number pad are drawn as their
// shapes and, like fn, no device has them.
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
	const char*	aszIconNames[] = { "Print Screen", "Scroll Lock", "Pause/Break" };
	const char*	aszNavigationTop[] = { "ins", "home", "pgup", NULL };
	const char*	aszNavigationTopNames[] = { "Insert", "Home", "Page Up" };
	const char*	aszNavigationBottom[] = { "del", "end", "pgdn", NULL };
	const char*	aszNavigationBottomNames[] = { "Delete", "End", "Page Down" };
	const char*	aszArrows[] = { "left", "down", "right", NULL };
	const char*	aszArrowNames[] = { "Cursor Left", "Cursor Down", "Cursor Right" };
	const char*	aszPadTop[] = { "clear", "/", "*", "-", NULL };
	const char*	aszPadTopNames[] = { "Num Lock", "Numpad /", "Numpad *", "Numpad -" };
	const char*	aszPad7[] = { "7", "8", "9", NULL };
	const char*	aszPad7Names[] = { "Numpad 7", "Numpad 8", "Numpad 9" };
	const char*	aszPad4[] = { "4", "5", "6", NULL };
	const char*	aszPad4Names[] = { "Numpad 4", "Numpad 5", "Numpad 6" };
	const char*	aszPad1[] = { "1", "2", "3", NULL };
	const char*	aszPad1Names[] = { "Numpad 1", "Numpad 2", "Numpad 3" };
	float32		x;
	float32		y;
	float32		fNavigation;
	float32		fPad;

	PreInit();

	maKeys.Init();
	miChanges = 0;

	fNavigation = 15 + KEYBOARD_GAP;
	fPad = fNavigation + 3 + KEYBOARD_GAP;
	msSize.Init(fPad + 4, 6 + KEYBOARD_GAP);

	AddKey(0, 0, 1, 1, "esc", "Escape");
	AddRow(2, 0, aszF1, NULL);
	AddRow(6.5f, 0, aszF5, NULL);
	AddRow(11, 0, aszF9, NULL);
	AddRow(fNavigation, 0, aszIcons, aszIconNames);
	AddKey(fPad, 0, KS_Circle);
	AddKey(fPad + 1, 0, KS_Triangle);
	AddKey(fPad + 2, 0, KS_Square);
	AddKey(fPad + 3, 0, KS_Cross);

	y = 1 + KEYBOARD_GAP;
	x = AddRow(0, y, aszNumbers, NULL);
	AddKey(x, y, 2, 1, "backspace", "Backspace");
	AddRow(fNavigation, y, aszNavigationTop, aszNavigationTopNames);
	AddRow(fPad, y, aszPadTop, aszPadTopNames);

	y += 1;
	AddKey(0, y, 1.5f, 1, "tab", "Tab");
	x = AddRow(1.5f, y, aszTop, NULL);
	AddKey(x, y, 1.5f, 1, "\\", "\\");
	AddRow(fNavigation, y, aszNavigationBottom, aszNavigationBottomNames);
	x = AddRow(fPad, y, aszPad7, aszPad7Names);
	AddKey(x, y, 1, 2, "+", "Numpad +");

	y += 1;
	AddKey(0, y, 1.75f, 1, "caps lock", "Caps Lock");
	x = AddRow(1.75f, y, aszHome, NULL);
	AddKey(x, y, 2.25f, 1, "enter", "Enter");
	AddRow(fPad, y, aszPad4, aszPad4Names);

	y += 1;
	AddKey(0, y, 2.25f, 1, "shift", "Left Shift");
	x = AddRow(2.25f, y, aszBottom, NULL);
	AddKey(x, y, 2.75f, 1, "shift", "Right Shift");
	AddKey(fNavigation + 1, y, 1, 1, "up", "Cursor Up");
	x = AddRow(fPad, y, aszPad1, aszPad1Names);
	AddKey(x, y, 1, 2, "enter", "Numpad Enter");

	y += 1;
	AddKey(0, y, 1.25f, 1, "ctrl", "Left Control");
	AddKey(1.25f, y, 1.25f, 1, "option", "Left Alt");
	AddKey(2.5f, y, 1.25f, 1, "cmd", "Left Special");
	AddKey(3.75f, y, 6.25f, 1, "", "Space");
	AddKey(10, y, 1.25f, 1, "cmd", "Right Special");
	AddKey(11.25f, y, 1.25f, 1, "option", "Right Alt");
	AddKey(12.5f, y, 1.25f, 1, "fn", NULL);
	AddKey(13.75f, y, 1.25f, 1, "ctrl", "Right Control");
	AddRow(fNavigation, y, aszArrows, aszArrowNames);
	AddKey(fPad, y, 2, 1, "0", "Numpad 0");
	AddKey(fPad + 2, y, 1, 1, ".", "Numpad .");

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
	U_Int32(miChanges);
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
void CKeyboard::AddKey(float32 x, float32 y, float32 fWidth, float32 fHeight, const char* szText, const char* szInputName)
{
	maKeys.Add(OMalloc<CKey>(x, y, fWidth, fHeight, OMalloc<CTextKeyDraw>(szText), szInputName));
}


//////////////////////////////////////////////////////////////////////////
//
// A one unit key with a shape for its legend.
//
//////////////////////////////////////////////////////////////////////////
void CKeyboard::AddKey(float32 x, float32 y, EKeyShape eShape)
{
	maKeys.Add(OMalloc<CKey>(x, y, 1.0f, 1.0f, OMalloc<CShapeKeyDraw>(eShape), (const char*)NULL));
}


//////////////////////////////////////////////////////////////////////////
//
// Adds one unit keys from x along the row until the NULL text.  Each
// key's input name is the one in aszInputNames, or its text if that's
// NULL.  Returns the x after the last key.
//
//////////////////////////////////////////////////////////////////////////
float32 CKeyboard::AddRow(float32 x, float32 y, const char** aszTexts, const char** aszInputNames)
{
	size	i;

	for (i = 0; aszTexts[i] != NULL; i++)
	{
		AddKey(x, y, 1, 1, aszTexts[i], aszInputNames ? aszInputNames[i] : aszTexts[i]);
		x += 1;
	}
	return x;
}


//////////////////////////////////////////////////////////////////////////
//
// Every key with the input name, as some names could be on two keys.
//
//////////////////////////////////////////////////////////////////////////
void CKeyboard::SetPressed(const char* szInputName, bool bPressed)
{
	size		i;
	Ptr<CKey>	pKey;

	if (StrEmpty(szInputName))
	{
		return;
	}

	for (i = 0; i < maKeys.Size(); i++)
	{
		pKey = maKeys.Get(i);
		if ((pKey->IsPressed() != bPressed) && (StringCompare(pKey->GetInputName(), szInputName) == 0))
		{
			pKey->SetPressed(bPressed);
			miChanges++;
		}
	}
}


//////////////////////////////////////////////////////////////////////////
//
// Keys let go of while another window has focus are never heard.
//
//////////////////////////////////////////////////////////////////////////
void CKeyboard::ReleaseAll(void)
{
	size		i;
	Ptr<CKey>	pKey;

	for (i = 0; i < maKeys.Size(); i++)
	{
		pKey = maKeys.Get(i);
		if (pKey->IsPressed())
		{
			pKey->SetPressed(false);
			miChanges++;
		}
	}
}


//////////////////////////////////////////////////////////////////////////
//
// The input action for every key down and up.  The event is passed on so
// the key still types or gives its command.
//
//////////////////////////////////////////////////////////////////////////
void CKeyboard::KeyAction(SComponentInputEvent* psEvent)
{
	SetPressed(psEvent->szName, psEvent->eType == CIET_KeyDown);
	psEvent->bPass = true;
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
size CKeyboard::NumKeys(void) { return maKeys.Size(); }
Ptr<CKey> CKeyboard::GetKey(size uiIndex) { return maKeys.Get(uiIndex); }
SFloat32Vec2 CKeyboard::GetSize(void) { return msSize; }
int32 CKeyboard::GetChanges(void) { return miChanges; }
