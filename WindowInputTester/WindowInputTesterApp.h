#ifndef __WINDOW_INPUT_TESTER_APP_H__
#define __WINDOW_INPUT_TESTER_APP_H__
#include "StandardLib/Object.h"
#include "StandardLib/Pointer.h"
#include "SupportLib/FontColours.h"
#include "InputLib/Input.h"
#include "InputLib/NativeInput.h"
#include "WindowLib/Window.h"
#include "WindowLib/PasteHistoryDialog.h"
#include "WindowLib/WindowTheme.h"
#include "WindowLib/TabbedContainer.h"
#include "WindowLib/MenuBar.h"
#include "WindowLib/Dialog.h"
#include "WindowLib/WindowInputSource.h"
#include "WindowLib/ComponentInput.h"
#include "WindowLib/NativeWindowFactory.h"
#include "WindowInputTesterTick.h"
#include "FileMenuListener.h"
#include "TabNameListener.h"


//The window input tester: its window (tabs under a menu bar, none to start with: File's Input Tester adds the mouse and the
//keyboard, and New and Open add editors and VRML views), the menu and tab listeners and the input that drives it.  main makes
//it, adds it to the root, runs it and removes it from the root.
class CWindowInputTesterApp : public CObject
{
CONSTRUCTABLE(CWindowInputTesterApp);
DESTRUCTABLE(CWindowInputTesterApp);
protected:
	Ptr<CWindow>				mpWindow;
	Ptr<CFontColours>			mpFontColours;  //msDialogLook's dim font is one of its fonts.
	Ptr<CWindowTheme>			mpTheme;  //msDialogLook's theme.
	Ptr<CTabbedContainer>		mpTabbed;
	Ptr<CMenuBar>				mpMenuBar;
	Ptr<CFileMenuListener>		mpFileMenuListener;
	Ptr<CTabNameListener>		mpTabNameListener;
	SDialogLook					msDialogLook;
	Ptr<CPasteHistoryDialog>	mpPasteHistory;  //Paste from History (Control+Shift+V): what the clipboard's history chooses with.
	SWindowInputTesterDialogs	msDialogs;  //The window's tick shows the dialogs the listeners ask for.
	Ptr<CInput>					mpInput;
	Ptr<CWindowInputSource>		mpInputSource;
	CComponentInput				mcComponentInput;
	bool						mbValid;
	bool						mbInputs;  //The listeners and the component input are set up.

public:
	void	Init(void);
	bool	Setup(CNativeWindowFactory* pcNativeFactory, CNativeInput* pcNativeInput, const char* szInputDirectory);
	void	Class(void) override;
	void	Free(void) override;

	bool	Save(CObjectWriter* pcFile) override;
	bool	Load(CObjectReader* pcFile) override;

	bool	Run(void);
};


void	InitWindowInputTesterGlobals(void);
void	KillWindowInputTesterGlobals(void);
bool	RunWindowInputTesterApp(CNativeWindowFactory* pcNativeFactory, CNativeInput* pcNativeInput, const char* szInputDirectory);


#endif // __WINDOW_INPUT_TESTER_APP_H__

