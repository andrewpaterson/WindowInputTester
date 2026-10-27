#ifndef __MOUSE_H__
#define __MOUSE_H__
#include "StandardLib/Object.h"
#include "WindowLib/ComponentInputEvent.h"


#define MOUSE_PRESSED_LEFT		0x01
#define MOUSE_PRESSED_RIGHT		0x02
#define MOUSE_PRESSED_WHEEL		0x04
#define MOUSE_ROLLED_UP			0x08
#define MOUSE_ROLLED_DOWN		0x10


//What a mouse's buttons and wheel are doing, as KeyAction hears them from the input devices: button 1 is the left
//button, 2 the right and 3 the wheel's.  The wheel has no release so it counts as rolled for a moment after each roll.
class CMouse : public CObject
{
CONSTRUCTABLE(CMouse);
DESTRUCTABLE(CMouse);
protected:
	bool	mbLeft;
	bool	mbRight;
	bool	mbWheel;
	int64	miRolledUp;  //When the wheel was last rolled forward, in milliseconds, 0 if it hasn't been.
	int64	miRolledDown;

public:
	void	Init(void);
	void	Class(void) override;
	void	Free(void) override;
	bool	Save(CObjectWriter* pcFile) override;
	bool	Load(CObjectReader* pcFile) override;

	void	KeyAction(SComponentInputEvent* psEvent);
	void	ReleaseAll(void);
	int32	GetState(int64 iNow);
};


#endif // __MOUSE_H__

