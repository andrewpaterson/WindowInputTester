#ifndef __WINDOW_INPUT_TESTER_TICK_H__
#define __WINDOW_INPUT_TESTER_TICK_H__
#include "WindowLib/WindowTick.h"


class CFileMenuListener;
class CTabNameListener;


//Dialogs are shown from the window's tick, not from the listener of the menu item or tab that asked for them, because
//a modal dialog reads the input itself.  The listeners remember what was asked for and the tick shows it.
struct SWindowInputTesterDialogs
{
	CFileMenuListener*	pcFileMenu;
	CTabNameListener*	pcTabNames;
};


class CWindowInputTesterTick : public CWindowTick
{
CONSTRUCTABLE(CWindowInputTesterTick);
DESTRUCTABLE(CWindowInputTesterTick);
protected:
	int							miSnapshotTicks;  //Ticks until the snapshot is saved, -1 once it has been.
	SWindowInputTesterDialogs*	mpsDialogs;

public:
	void	Init(SWindowInputTesterDialogs* psDialogs);
	void	Class(void) override;
	void	Free(void) override;
	void	Tick(Ptr<CWindow> pWindow, int64 iUpdateTimeInMillieconds, int64 iTotalTimeInMillieconds) override;
};


#endif // __WINDOW_INPUT_TESTER_TICK_H__
