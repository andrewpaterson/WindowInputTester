#include "StandardLib/Objects.h"
#include "StandardLib/ObjectReader.h"
#include "StandardLib/ObjectWriter.h"
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
	Ptr<CDialog>		pDialog;
	Ptr<CTextField>		pName;
	bool				bResult;

	//Nothing else holds the dialog so it's in the root while it's shown.
	pDialog = OMalloc<CDialog>(pOwner, psLook, szTitle);
	ORoot()->Add(pDialog);
	pName = pDialog->AddTextField("Name", szName);
	bResult = pDialog->Show(TAB_NAME_DIALOG_WIDTH) && (pName->GetText()[0] != '\0');
	if (bResult)
	{
		pszName->Set(pName->GetText());
	}

	pName = NULL;
	ORoot()->Remove(pDialog);
	pDialog = NULL;
	return bResult;
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CTabNameListener::Init(Ptr<CWindow> pOwner, SDialogLook* psLook, Ptr<CTabComponent> pInputTab)
{
	PreInit();

	mpOwner = pOwner;
	mpsLook = psLook;
	mpInputTab = pInputTab;
	mpRenameTab = NULL;

	PostInit();
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CTabNameListener::Class(void)
{
	M_Pointer(mpOwner);
	U_Pointer(mpsLook);
	M_Pointer(mpInputTab);
	M_Pointer(mpRenameTab);
}


//////////////////////////////////////////////////////////////////////////
//
// A listener isn't saved.
//
//////////////////////////////////////////////////////////////////////////
bool CTabNameListener::Save(CObjectWriter* pcFile)
{
	return false;
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
bool CTabNameListener::Load(CObjectReader* pcFile)
{
	return false;
}



//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CTabNameListener::Free(void)
{
}


//////////////////////////////////////////////////////////////////////////
//
// pcSource is the tab double clicked.  The Input tab keeps its name.
//
//////////////////////////////////////////////////////////////////////////
void CTabNameListener::TabDoubleClicked(CTabComponent* pcTab)
{
	if (&mpInputTab != pcTab)
	{
		mpRenameTab = pcTab;
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

