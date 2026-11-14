#include <math.h>
#include "BaseLib/StringHelper.h"
#include "BaseLib/Timer.h"
#include "StandardLib/ClassDefines.h"
#include "Mouse.h"


#define MOUSE_ROLLED_MILLISECONDS	300  //How long the wheel counts as rolled after a roll.
#define MOUSE_MOTION_MILLISECONDS	80.0  //How quickly the velocity follows the movement; it falls to a third this long after the mouse stops.
#define MOUSE_STILL_MILLISECONDS	200  //How long after it last moved the mouse is still, whatever its velocity.


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CMouse::Init(void)
{
	PreInit();

	mbLeft = false;
	mbRight = false;
	mbWheel = false;
	miRolledUp = 0;
	miRolledDown = 0;
	miMovedRight = 0;
	miMovedDown = 0;
	mfVelocityRight = 0;
	mfVelocityDown = 0;
	miMotionTime = 0;
	miMovedTime = 0;

	PostInit();
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CMouse::Class(void)
{
	U_Bool(mbLeft);
	U_Bool(mbRight);
	U_Bool(mbWheel);
	U_Int64(miRolledUp);
	U_Int64(miRolledDown);
	U_Int32(miMovedRight);
	U_Int32(miMovedDown);
	U_Float64(mfVelocityRight);
	U_Float64(mfVelocityDown);
	U_Int64(miMotionTime);
	U_Int64(miMovedTime);
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CMouse::Free(void)
{
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
bool CMouse::Save(CObjectWriter* pcFile)
{
	return false;
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
bool CMouse::Load(CObjectReader* pcFile)
{
	return false;
}


//////////////////////////////////////////////////////////////////////////
//
// The buttons are down if any mouse of pcInput has them down.
//
//////////////////////////////////////////////////////////////////////////
void CMouse::Update(CComponentInput* pcInput)
{
	mbLeft = pcInput->IsSourceDown("Button 1");
	mbRight = pcInput->IsSourceDown("Button 2");
	mbWheel = pcInput->IsSourceDown("Button 3");
}


//////////////////////////////////////////////////////////////////////////
//
// The input action for every key and button down.  The event is passed on
// so the wheel still scrolls.
//
//////////////////////////////////////////////////////////////////////////
void CMouse::KeyAction(SComponentInputEvent* psEvent)
{
	psEvent->bPass = true;
	if (psEvent->szName == NULL)
	{
		return;
	}

	if (StringCompare(psEvent->szName, "Wheel Forward") == 0)
	{
		miRolledUp = GetDebugTickCount();
	}
	else if (StringCompare(psEvent->szName, "Wheel Backward") == 0)
	{
		miRolledDown = GetDebugTickCount();
	}
}


//////////////////////////////////////////////////////////////////////////
//
// The input action for movement.  The event is passed on so the pointer
// still moves over the components.
//
//////////////////////////////////////////////////////////////////////////
void CMouse::MovementAction(SComponentInputEvent* psEvent)
{
	psEvent->bPass = true;
	miMovedRight += psEvent->iRight;
	miMovedDown += psEvent->iDown;
	if ((psEvent->iRight != 0) || (psEvent->iDown != 0))
	{
		miMovedTime = GetDebugTickCount();
	}
}


//////////////////////////////////////////////////////////////////////////
//
// The movement since the last call, over the time since it, is blended
// into the velocity; the longer the time the more it counts.  It's 0 once
// the mouse hasn't moved for MOUSE_STILL_MILLISECONDS.  iNow is in
// milliseconds, as GetDebugTickCount.
//
//////////////////////////////////////////////////////////////////////////
void CMouse::UpdateMotion(int64 iNow)
{
	float64	fElapsed;
	float64	fKeep;

	if (miMotionTime == 0)
	{
		miMotionTime = iNow;
		miMovedRight = 0;
		miMovedDown = 0;
		return;
	}

	fElapsed = (float64)(iNow - miMotionTime);
	if (fElapsed <= 0)
	{
		return;
	}

	fKeep = exp(-fElapsed / MOUSE_MOTION_MILLISECONDS);
	mfVelocityRight = mfVelocityRight * fKeep + (miMovedRight * 1000.0 / fElapsed) * (1 - fKeep);
	mfVelocityDown = mfVelocityDown * fKeep + (miMovedDown * 1000.0 / fElapsed) * (1 - fKeep);
	if (iNow - miMovedTime >= MOUSE_STILL_MILLISECONDS)
	{
		mfVelocityRight = 0;
		mfVelocityDown = 0;
	}
	miMovedRight = 0;
	miMovedDown = 0;
	miMotionTime = iNow;
}


//////////////////////////////////////////////////////////////////////////
//
// Whether what's drawn will change at iNow with no more input: it has a
// velocity, which fades, or the wheel counts as rolled, which goes out.
//
//////////////////////////////////////////////////////////////////////////
bool CMouse::IsChanging(int64 iNow)
{
	return (mfVelocityRight != 0) || (mfVelocityDown != 0) || ((GetState(iNow) & (MOUSE_ROLLED_UP | MOUSE_ROLLED_DOWN)) != 0);
}


//////////////////////////////////////////////////////////////////////////
//
// Buttons let go of while another window has focus are never heard, and
// it hears no movement.
//
//////////////////////////////////////////////////////////////////////////
void CMouse::ReleaseAll(void)
{
	mbLeft = false;
	mbRight = false;
	mbWheel = false;
	miMovedRight = 0;
	miMovedDown = 0;
	mfVelocityRight = 0;
	mfVelocityDown = 0;
	miMotionTime = 0;
}


//////////////////////////////////////////////////////////////////////////
//
// The MOUSE_PRESSED_ and MOUSE_ROLLED_ flags at iNow (milliseconds, as
// GetDebugTickCount).
//
//////////////////////////////////////////////////////////////////////////
int32 CMouse::GetState(int64 iNow)
{
	int32	iState;

	iState = 0;
	if (mbLeft)
	{
		iState |= MOUSE_PRESSED_LEFT;
	}
	if (mbRight)
	{
		iState |= MOUSE_PRESSED_RIGHT;
	}
	if (mbWheel)
	{
		iState |= MOUSE_PRESSED_WHEEL;
	}
	if ((miRolledUp != 0) && (iNow - miRolledUp < MOUSE_ROLLED_MILLISECONDS))
	{
		iState |= MOUSE_ROLLED_UP;
	}
	if ((miRolledDown != 0) && (iNow - miRolledDown < MOUSE_ROLLED_MILLISECONDS))
	{
		iState |= MOUSE_ROLLED_DOWN;
	}
	return iState;
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
float64 CMouse::GetVelocityRight(void) { return mfVelocityRight; }
float64 CMouse::GetVelocityDown(void) { return mfVelocityDown; }

