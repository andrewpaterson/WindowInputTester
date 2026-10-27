#ifndef __KEY_H__
#define __KEY_H__
#include "BaseLib/Chars.h"
#include "BaseLib/Float32Vec2.h"
#include "StandardLib/Object.h"
#include "KeyDraw.h"


//One key of a keyboard.  Its position (top left) and size are in key units, a normal key is one unit square.  Its draw
//draws its legend.  Its input name is the name of the key the input devices give, like "Left Shift", empty for a key
//no device has.
class CKey : public CObject
{
CONSTRUCTABLE(CKey);
DESTRUCTABLE(CKey);
protected:
	SFloat32Vec2	msPosition;
	SFloat32Vec2	msSize;
	Ptr<CKeyDraw>	mpDraw;
	CChars			mszInputName;
	bool			mbPressed;

public:
	void			Init(float32 x, float32 y, float32 fWidth, float32 fHeight, Ptr<CKeyDraw> pDraw, const char* szInputName);
	void			Class(void) override;
	void			Free(void) override;
	bool			Save(CObjectWriter* pcFile) override;
	bool			Load(CObjectReader* pcFile) override;

	SFloat32Vec2	GetPosition(void);
	SFloat32Vec2	GetSize(void);
	Ptr<CKeyDraw>	GetDraw(void);
	char*			GetInputName(void);
	bool			IsPressed(void);
	void			SetPressed(bool bPressed);
};


#endif // __KEY_H__
