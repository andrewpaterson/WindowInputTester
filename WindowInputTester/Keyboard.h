#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__
#include "StandardLib/Array.h"
#include "WindowLib/ComponentInput.h"
#include "ShapeKeyDraw.h"
#include "Key.h"


//The keys of a keyboard laid out in key units.  Init lays out a full size ANSI keyboard like the Keychron V5 Max (see
//Input/Keychron v5 max.png) with its legends: the main keys under the escape and function row, the navigation keys
//and arrows beside them, and the number pad, each with a row of icon keys above.
//
//A key is pressed while the input devices say the key of its input name is down, as Update reads them.  The keys
//are read from the devices rather than heard as events because the chords hold back the events of keys that could
//start a chord, like shift and control, while they are down.  Either side of the space bar the keys are those of
//the keyboard in Windows mode: ctrl is Control, option the Windows key (Special) and alt Alt.
class CKeyboard : public CObject
{
CONSTRUCTABLE(CKeyboard);
DESTRUCTABLE(CKeyboard);
protected:
	CArray<CKey>	maKeys;
	SFloat32Vec2	msSize;  //Of the whole keyboard, in key units.
	int32			miChanges;  //Counts the keys pressed and released, so a draw knows when to draw them again.

public:
	void			Init(void);
	void			Class(void) override;
	void			Free(void) override;
	bool			Save(CObjectWriter* pcFile) override;
	bool			Load(CObjectReader* pcFile) override;

	size			NumKeys(void);
	Ptr<CKey>		GetKey(size uiIndex);
	SFloat32Vec2	GetSize(void);
	int32			GetChanges(void);

	void			Update(CComponentInput* pcInput);
	void			ReleaseAll(void);

protected:
	void			AddKey(float32 x, float32 y, float32 fWidth, float32 fHeight, const char* szText, const char* szInputName);
	void			AddKey(float32 x, float32 y, EKeyShape eShape);
	float32			AddRow(float32 x, float32 y, const char** aszTexts, const char** aszInputNames);
};


#endif // __KEYBOARD_H__
