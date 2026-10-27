#ifndef __KEY_H__
#define __KEY_H__
#include "BaseLib/Float32Vec2.h"
#include "StandardLib/Object.h"
#include "KeyDraw.h"


//One key of a keyboard.  Its position (top left) and size are in key units, a normal key is one unit square.  Its draw
//draws its legend.
class CKey : public CObject
{
CONSTRUCTABLE(CKey);
DESTRUCTABLE(CKey);
protected:
	SFloat32Vec2	msPosition;
	SFloat32Vec2	msSize;
	Ptr<CKeyDraw>	mpDraw;

public:
	void			Init(float32 x, float32 y, float32 fWidth, float32 fHeight, Ptr<CKeyDraw> pDraw);
	void			Class(void) override;
	void			Free(void) override;
	bool			Save(CObjectWriter* pcFile) override;
	bool			Load(CObjectReader* pcFile) override;

	SFloat32Vec2	GetPosition(void);
	SFloat32Vec2	GetSize(void);
	Ptr<CKeyDraw>	GetDraw(void);
};


#endif // __KEY_H__
