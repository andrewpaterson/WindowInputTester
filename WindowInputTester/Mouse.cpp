#include "BaseLib/StringHelper.h"
#include "BaseLib/Timer.h"
#include "StandardLib/ClassDefines.h"
#include "Mouse.h"


#define MOUSE_ROLLED_MILLISECONDS	300  //How long the wheel counts as rolled after a roll.


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
// The input action for every key and button down and up.  The event is
// passed on so the button still clicks and the wheel still scrolls.
//
//////////////////////////////////////////////////////////////////////////
void CMouse::KeyAction(SComponentInputEvent* psEvent)
{
	bool	bDown;

	psEvent->bPass = true;
	if (psEvent->szName == NULL)
	{
		return;
	}

	bDown = psEvent->eType == CIET_KeyDown;
	if (StringCompare(psEvent->szName, "Button 1") == 0)
	{
		mbLeft = bDown;
	}
	else if (StringCompare(psEvent->szName, "Button 2") == 0)
	{
		mbRight = bDown;
	}
	else if (StringCompare(psEvent->szName, "Button 3") == 0)
	{
		mbWheel = bDown;
	}
	else if (bDown && (StringCompare(psEvent->szName, "Wheel Forward") == 0))
	{
		miRolledUp = GetDebugTickCount();
	}
	else if (bDown && (StringCompare(psEvent->szName, "Wheel Backward") == 0))
	{
		miRolledDown = GetDebugTickCount();
	}
}


//////////////////////////////////////////////////////////////////////////
//
// Buttons let go of while another window has focus are never heard.
//
//////////////////////////////////////////////////////////////////////////
void CMouse::ReleaseAll(void)
{
	mbLeft = false;
	mbRight = false;
	mbWheel = false;
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

