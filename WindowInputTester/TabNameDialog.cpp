#include "StandardLib/Objects.h"
#include "WindowLib/ClearCanvasDraw.h"
#include "WindowLib/FillContainer.h"
#include "WindowLib/GridContainer.h"
#include "WindowLib/ButtonBar.h"
#include "WindowLib/Label.h"
#include "WindowLib/JewelColours.h"
#include "TabNameDialog.h"


#define TAB_NAME_DIALOG_WIDTH		360
#define TAB_NAME_DIALOG_INSET		16  //Between the window's edge and the row and buttons.
#define TAB_NAME_DIALOG_LABEL_GAP	12  //Between the label and the text field.
#define TAB_NAME_DIALOG_BUTTON_GAP	16  //Between the row and the buttons.


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CTabNameDialogTick::Init(void)
{
	PreInit();
	CWindowTick::Init();
	PostInit();
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CTabNameDialogTick::Tick(Ptr<CWindow> pWindow, int64 iUpdateTimeInMillieconds, int64 iTotalTimeInMillieconds)
{
	pWindow->Paint();
}


//////////////////////////////////////////////////////////////////////////
//
// The dialog's window shares the owner's native window factory and
// colour format.  The text field starts with szName selected so typing
// replaces it.
//
//////////////////////////////////////////////////////////////////////////
void CTabNameDialog::Init(Ptr<CWindow> pOwner, Ptr<CWindowTheme> pTheme, ARGB32 sBackground, const char* szName)
{
	Ptr<CFillContainer>		pFill;
	Ptr<CGridContainer>		pGrid;
	Ptr<CGridContainer>		pRow;
	Ptr<CLabel>				pLabel;
	Ptr<CButtonBar>			pButtons;
	SInsets					sInsets;
	int						aiInsets[4] = { TAB_NAME_DIALOG_INSET, TAB_NAME_DIALOG_INSET, TAB_NAME_DIALOG_INSET, TAB_NAME_DIALOG_INSET };

	mpOwner = pOwner;
	mbOK = false;

	mpDialog = OMalloc<CWindow>("Rename Tab", pOwner->GetFactory(), OMalloc<CTabNameDialogTick>(), OMalloc<CClearCanvasDraw>(sBackground), pOwner->GetColourFormat(), pOwner->GetColourOrder(), pOwner->GetColourBits(), pOwner->GetAlphaBits());
	ORoot()->Add(mpDialog);

	pFill = OMalloc<CFillContainer>(mpDialog);
	sInsets.Init(aiInsets);
	pFill->SetInsets(&sInsets);
	mpDialog->SetContainer(pFill);

	//The row at the top and the buttons at the bottom.
	pGrid = OMalloc<CGridContainer>(mpDialog);
	pGrid->SetAlignment(CSH_Stretch, CSV_Stretch);
	pFill->AddComponent(pGrid);

	pRow = OMalloc<CGridContainer>(mpDialog);
	pRow->SetAlignment(CSH_Stretch, CSV_Top);
	pGrid->AddComponent(pRow, 0, 0, 1, 1, 1, 1);

	pLabel = OMalloc<CLabel>(mpDialog, pTheme->GetFont(), "Name");
	pLabel->SetDesiredSize(pLabel->GetRequiredSize().x + TAB_NAME_DIALOG_LABEL_GAP, -1);
	pLabel->SetAlignment(CSH_Left, CSV_Center);
	pRow->AddComponent(pLabel, 0, 0);

	mpName = OMalloc<CTextField>(mpDialog, pTheme, szName);
	mpName->SetColumns(10);
	mpName->SelectAll();
	mpName->SetAlignment(CSH_Stretch, CSV_Center);
	pRow->AddComponent(mpName, 1, 0, 1, 1, 1, 0);

	mpOK = OMalloc<CButton>(mpDialog, pTheme, "OK");
	mpCancel = OMalloc<CButton>(mpDialog, pTheme, "Cancel");
	mpOK->SetDesiredSize(72, -1);
	mpCancel->SetDesiredSize(72, -1);
	mpOK->AddListener(this);
	mpCancel->AddListener(this);
	mpDialog->SetDefaultButton(mpOK);
	mpDialog->SetCancelButton(mpCancel);

	pButtons = OMalloc<CButtonBar>(mpDialog);
	pButtons->SetInsets(0, 0, 0, 0);
	pButtons->SetAlignment(CSH_Stretch, CSV_Bottom);
	pButtons->AddComponent(mpOK);
	pButtons->AddComponent(mpCancel);
	pGrid->AddComponent(pButtons, 0, 1, 1, 1, 1, 0);

	pRow->SetRequiredSize();
	miHeight = TAB_NAME_DIALOG_INSET + pRow->GetRequiredSize().y + TAB_NAME_DIALOG_BUTTON_GAP + mpOK->GetDesiredSize().y + TAB_NAME_DIALOG_INSET;
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CTabNameDialog::Kill(void)
{
	mpOK->RemoveListener(this);
	mpCancel->RemoveListener(this);

	mpOK = NULL;
	mpCancel = NULL;
	mpName = NULL;
	ORoot()->Remove(mpDialog);
	mpDialog = NULL;
	mpOwner = NULL;
}


//////////////////////////////////////////////////////////////////////////
//
// Returns true if OK closed the dialog.
//
//////////////////////////////////////////////////////////////////////////
bool CTabNameDialog::Show(void)
{
	mpDialog->SetWindowSize(TAB_NAME_DIALOG_WIDTH, miHeight);
	mpDialog->GetFocus()->SetFocus(mpName);

	mbOK = false;
	mpDialog->ShowModal(mpOwner);
	return mbOK;
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
char* CTabNameDialog::GetName(void)
{
	return mpName->GetText();
}


//////////////////////////////////////////////////////////////////////////
//
// Both buttons close the dialog.
//
//////////////////////////////////////////////////////////////////////////
void CTabNameDialog::ButtonClicked(CUnknown* pcSource, void* pvContext)
{
	mbOK = (&mpOK == pcSource);
	mpDialog->Stop();
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CTabNameListener::Init(Ptr<CWindow> pOwner, Ptr<CWindowTheme> pTheme, Ptr<CTabComponent> pInputTab)
{
	mpOwner = pOwner;
	mpTheme = pTheme;
	mpInputTab = pInputTab;
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CTabNameListener::Kill(void)
{
	mpInputTab = NULL;
	mpTheme = NULL;
	mpOwner = NULL;
}


//////////////////////////////////////////////////////////////////////////
//
// pcSource is the tab double clicked.  The Input tab keeps its name.
//
//////////////////////////////////////////////////////////////////////////
void CTabNameListener::TabDoubleClicked(CUnknown* pcSource, void* pvContext)
{
	CTabComponent*	pcTab;
	CTabNameDialog	cDialog;

	if (&mpInputTab == pcSource)
	{
		return;
	}

	pcTab = (CTabComponent*)pcSource;
	cDialog.Init(mpOwner, mpTheme, JEWEL_PANEL, pcTab->GetText());
	if (cDialog.Show())
	{
		pcTab->SetText(cDialog.GetName());
	}
	cDialog.Kill();
}

