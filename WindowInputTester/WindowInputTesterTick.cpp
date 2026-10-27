#include "StandardLib/ClassDefines.h"
#include "SupportLib/PNGWriter.h"
#include "WindowLib/Window.h"
#include "WindowInputTesterTick.h"


#define WINDOW_INPUT_TESTER_SNAPSHOT_TICKS		10
#define WINDOW_INPUT_TESTER_SNAPSHOT_FILENAME	"Output/Snapshot.png"


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CWindowInputTesterTick::Init(void)
{
	PreInit();

	CWindowTick::Init();
	miSnapshotTicks = WINDOW_INPUT_TESTER_SNAPSHOT_TICKS;

	PostInit();
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CWindowInputTesterTick::Class(void)
{
	CWindowTick::Class();
	U_Int32(miSnapshotTicks);
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CWindowInputTesterTick::Free(void)
{
	CWindowTick::Free();
}


//////////////////////////////////////////////////////////////////////////
//
// Saves the window as a PNG once it has been drawn a few times, to check
// how it draws without looking at the screen.  The window stays open.
//
//////////////////////////////////////////////////////////////////////////
void CWindowInputTesterTick::Tick(Ptr<CWindow> pWindow, int64 iUpdateTimeInMillieconds, int64 iTotalTimeInMillieconds)
{
	pWindow->Paint();

	if (miSnapshotTicks >= 0)
	{
		if (miSnapshotTicks == 0)
		{
			SavePNG(pWindow->GetCanvas()->GetImageOrNull(), WINDOW_INPUT_TESTER_SNAPSHOT_FILENAME);
		}
		miSnapshotTicks--;
	}
}
