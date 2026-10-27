#ifndef __WINDOW_INPUT_TESTER_TICK_H__
#define __WINDOW_INPUT_TESTER_TICK_H__
#include "WindowLib/WindowTick.h"


class CWindowInputTesterTick : public CWindowTick
{
CONSTRUCTABLE(CWindowInputTesterTick);
DESTRUCTABLE(CWindowInputTesterTick);
protected:
	int		miSnapshotTicks;  //Ticks until the snapshot is saved, -1 once it has been.

public:
	void	Init(void);
	void	Class(void) override;
	void	Free(void) override;
	void	Tick(Ptr<CWindow> pWindow, int64 iUpdateTimeInMillieconds, int64 iTotalTimeInMillieconds) override;
};


#endif // __WINDOW_INPUT_TESTER_TICK_H__
