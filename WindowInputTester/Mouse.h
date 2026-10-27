#ifndef __MOUSE_H__
#define __MOUSE_H__
#include "StandardLib/Object.h"
#include "WindowLib/ComponentInput.h"


#define MOUSE_PRESSED_LEFT		0x01
#define MOUSE_PRESSED_RIGHT		0x02
#define MOUSE_PRESSED_WHEEL		0x04
#define MOUSE_ROLLED_UP			0x08
#define MOUSE_ROLLED_DOWN		0x10


//What a mouse's buttons and wheel are doing: button 1 is the left button, 2 the right and 3 the wheel's.  The buttons
//are read from the devices by Update, as the chords hold back a button's event while a modifier is down (control and
//click).  A roll of the wheel is only an event, which KeyAction hears; the wheel has no release so it counts as rolled
//for a moment after each roll.
//
//Movement is heard by MovementAction as how far the mouse moved right and down since the last event, in the device's
//counts.  UpdateMotion turns what moved since it was last called into a velocity, smoothed so that it fades out over
//a fraction of a second once the mouse stops.
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
	int32	miMovedRight;  //Since the last UpdateMotion.
	int32	miMovedDown;
	float64	mfVelocityRight;  //Smoothed, in counts a second.
	float64	mfVelocityDown;
	int64	miMotionTime;  //When UpdateMotion was last called, 0 if it hasn't been.

public:
	void	Init(void);
	void	Class(void) override;
	void	Free(void) override;
	bool	Save(CObjectWriter* pcFile) override;
	bool	Load(CObjectReader* pcFile) override;

	void	Update(CComponentInput* pcInput);
	void	KeyAction(SComponentInputEvent* psEvent);
	void	MovementAction(SComponentInputEvent* psEvent);
	void	UpdateMotion(int64 iNow);
	void	ReleaseAll(void);
	int32	GetState(int64 iNow);
	float64	GetVelocityRight(void);
	float64	GetVelocityDown(void);
};


#endif // __MOUSE_H__

