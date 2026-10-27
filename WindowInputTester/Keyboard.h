#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__
#include "StandardLib/Array.h"
#include "ShapeKeyDraw.h"
#include "Key.h"


//The keys of a keyboard laid out in key units.  Init lays out a full size ANSI keyboard like the Keychron V5 Max (see
//Input/Keychron v5 max.png) with its Mac legends: the main keys under the escape and function row, the navigation keys
//and arrows beside them, and the number pad, each with a row of icon keys above.
class CKeyboard : public CObject
{
CONSTRUCTABLE(CKeyboard);
DESTRUCTABLE(CKeyboard);
protected:
	CArray<CKey>	maKeys;
	SFloat32Vec2	msSize;  //Of the whole keyboard, in key units.

public:
	void			Init(void);
	void			Class(void) override;
	void			Free(void) override;
	bool			Save(CObjectWriter* pcFile) override;
	bool			Load(CObjectReader* pcFile) override;

	size			NumKeys(void);
	Ptr<CKey>		GetKey(size uiIndex);
	SFloat32Vec2	GetSize(void);

protected:
	void			AddKey(float32 x, float32 y, float32 fWidth, float32 fHeight, const char* szText);
	void			AddKey(float32 x, float32 y, EKeyShape eShape);
	float32			AddRow(float32 x, float32 y, const char** aszTexts);
};


#endif // __KEYBOARD_H__
