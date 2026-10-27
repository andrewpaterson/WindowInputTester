#include "StandardLib/Objects.h"
#include "TabNameListener.h"


#define TAB_NAME_DIALOG_WIDTH	360


//////////////////////////////////////////////////////////////////////////
//
// Asks for a tab's name in a dialog titled szTitle, with a Name text field
// starting with szName selected so typing replaces it.  Returns true, the
// name in pszName (which must be initialised), if OK closed the dialog
// with a name.
//
//////////////////////////////////////////////////////////////////////////
bool AskTabName(Ptr<CWindow> pOwner, SDialogLook* psLook, const char* szTitle, const char* szName, CChars* pszName)
{
	CDialog				cDialog;
	Ptr<CTextField>		pName;
	bool				bResult;

	cDialog.Init(pOwner, psLook, szTitle);
	pName = cDialog.AddTextField("Name", szName);
	bResult = cDialog.Show(TAB_NAME_DIALOG_WIDTH) && (pName->GetText()[0] != '\0');
	if (bResult)
	{
		pszName->Set(pName->GetText());
	}

	pName = NULL;
	cDialog.Kill();
	return bResult;
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CTabNameListener::Init(Ptr<CWindow> pOwner, SDialogLook* psLook, Ptr<CTabComponent> pInputTab)
{
	mpOwner = pOwner;
	mpsLook = psLook;
	mpInputTab = pInputTab;
	mpRenameTab = NULL;
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CTabNameListener::Kill(void)
{
	mpRenameTab = NULL;
	mpInputTab = NULL;
	mpsLook = NULL;
	mpOwner = NULL;
}


//////////////////////////////////////////////////////////////////////////
//
// pcSource is the tab double clicked.  The Input tab keeps its name.
//
//////////////////////////////////////////////////////////////////////////
void CTabNameListener::TabDoubleClicked(CUnknown* pcSource, void* pvContext)
{
	if (&mpInputTab != pcSource)
	{
		mpRenameTab = (CTabComponent*)pcSource;
	}
}


//////////////////////////////////////////////////////////////////////////
//
// Called by the window's tick.  Asks for the new name of the tab double
// clicked, if any and it hasn't been closed since.
//
//////////////////////////////////////////////////////////////////////////
void CTabNameListener::ShowDialog(void)
{
	Ptr<CTabComponent>	pTab;
	CChars				szName;

	pTab = mpRenameTab;
	mpRenameTab = NULL;
	if (pTab.IsNull() || pTab->GetParent().IsNull())
	{
		return;
	}

	szName.Init();
	if (AskTabName(mpOwner, mpsLook, "Rename Tab", pTab->GetText(), &szName))
	{
		pTab->SetText(szName.Text());
	}
	szName.Kill();
}

