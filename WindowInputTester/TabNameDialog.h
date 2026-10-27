#ifndef __TAB_NAME_DIALOG_H__
#define __TAB_NAME_DIALOG_H__
#include "WindowLib/Window.h"
#include "WindowLib/WindowTick.h"
#include "WindowLib/WindowTheme.h"
#include "WindowLib/Button.h"
#include "WindowLib/ButtonListener.h"
#include "WindowLib/TextField.h"
#include "WindowLib/TabbedContainer.h"


//Paints the dialog each tick.
class CTabNameDialogTick : public CWindowTick
{
CONSTRUCTABLE(CTabNameDialogTick);
DESTRUCTABLE(CTabNameDialogTick);
public:
	void	Init(void);
	void	Tick(Ptr<CWindow> pWindow, int64 iUpdateTimeInMillieconds, int64 iTotalTimeInMillieconds) override;
};


//A Name label and a text field holding a tab's name in a window of its own titled szTitle, shown modally over the
//tester's window in IntelliJ's style with OK and Cancel along the bottom on the right.  OK is the default button, enter
//clicks it; escape clicks Cancel.  The dialog's window is in the root while the dialog exists.
class CTabNameDialog : public CButtonListener
{
CONSTRUCTABLE(CTabNameDialog);
protected:
	Ptr<CWindow>		mpOwner;
	Ptr<CWindow>		mpDialog;
	Ptr<CTextField>		mpName;
	Ptr<CButton>		mpOK;
	Ptr<CButton>		mpCancel;
	int32				miHeight;
	bool				mbOK;

public:
	void	Init(Ptr<CWindow> pOwner, Ptr<CWindowTheme> pTheme, ARGB32 sBackground, const char* szTitle, const char* szName);
	void	Kill(void);

	bool	Show(void);
	char*	GetName(void);

	void	ButtonClicked(CUnknown* pcSource, void* pvContext) override;
};


//Double clicking a tab other than the Input tab asks for its new name with a CTabNameDialog.  OK renames it unless
//the name is empty.
class CTabNameListener : public CTabbedContainerListener
{
CONSTRUCTABLE(CTabNameListener);
protected:
	Ptr<CWindow>		mpOwner;
	Ptr<CWindowTheme>	mpTheme;
	Ptr<CTabComponent>	mpInputTab;

public:
	void	Init(Ptr<CWindow> pOwner, Ptr<CWindowTheme> pTheme, Ptr<CTabComponent> pInputTab);
	void	Kill(void);

	void	TabDoubleClicked(CUnknown* pcSource, void* pvContext) override;
};


#endif // __TAB_NAME_DIALOG_H__

