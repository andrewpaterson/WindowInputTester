#ifndef __TAB_NAME_LISTENER_H__
#define __TAB_NAME_LISTENER_H__
#include "BaseLib/Chars.h"
#include "WindowLib/Dialog.h"
#include "WindowLib/TabbedContainer.h"


//Double clicking a tab other than the Input tab asks for its new name with AskTabName.  OK renames it.  The double
//click only asks for the dialog; the window's tick shows it with ShowDialog.
class CTabNameListener : public CObject, public CTabbedContainerListener
{
CONSTRUCTABLE(CTabNameListener);
DESTRUCTABLE(CTabNameListener);
protected:
	Ptr<CWindow>		mpOwner;
	SDialogLook*		mpsLook;
	Ptr<CTabComponent>	mpInputTab;
	Ptr<CTabComponent>	mpRenameTab;  //The tab to ask the name of next tick, NULL for none.

public:
	void	Init(Ptr<CWindow> pOwner, SDialogLook* psLook, Ptr<CTabComponent> pInputTab);
	void	Class(void) override;
	void	Free(void) override;

	bool	Save(CObjectWriter* pcFile) override;
	bool	Load(CObjectReader* pcFile) override;

	void	TabDoubleClicked(CUnknown* pcSource, void* pvContext) override;
	void	ShowDialog(void);
};


bool AskTabName(Ptr<CWindow> pOwner, SDialogLook* psLook, const char* szTitle, const char* szName, CChars* pszName);


#endif // __TAB_NAME_LISTENER_H__

