#include "BaseLib/Logger.h"
#include "BaseLib/StringHelper.h"
#include "BaseLib/TextFile.h"
#include "StandardLib/ClassDefines.h"
#include "WindowLib/Canvas.h"
#include "WindowLib/ClearCanvasDraw.h"
#include "WindowLib/ScrollPane.h"
#include "EditorPage.h"


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CEditorPage::Init(Ptr<CWindow> pWindow, Ptr<CWindowTheme> pTheme, Ptr<CFontColours> pFontColours, const char* szText)
{
	Ptr<CScrollPane>	pScroll;
	Ptr<CCanvas>		pBackground;

	PreInit();

	CFillContainer::Init(pWindow);
	SetAlignment(CSH_Stretch, CSV_Stretch);
	mszFilename.Init();

	mpTextArea = OMalloc<CTextArea>(pWindow, pTheme, pFontColours, szText);
	mpTextArea->SetColour(pTheme->GetColour(TC_Text));
	mpTextArea->SetWordWrap(true);
	mpTextArea->SetAlignment(CSH_Stretch, CSV_Stretch);

	pScroll = OMalloc<CScrollPane>(pWindow, pTheme);
	pScroll->SetAlignment(CSH_Stretch, CSV_Stretch);
	pScroll->SetPolicy(SBO_Horizontal, SPP_Never);
	pScroll->SetContent(mpTextArea);

	pBackground = OMalloc<CCanvas>(pWindow, OMalloc<CClearCanvasDraw>(pTheme->GetColour(TC_Editor)));
	pBackground->SetAlignment(CSH_Stretch, CSV_Stretch);
	AddComponent(pBackground);
	AddComponent(pScroll);

	mpInsertMenu = OMalloc<CMenuContainer>(pWindow, pTheme);
	mpInsertMenu->AddItem("Hello");
	mpInsertMenu->AddItem("World");
	mpInsertMenu->AddItem("The quick brown fox");
	mpMenu = OMalloc<CMenuContainer>(pWindow, pTheme);
	mpCut = mpMenu->AddItem("Cut");
	mpCopy = mpMenu->AddItem("Copy");
	mpPaste = mpMenu->AddItem("Paste");
	mpMenu->AddSeparator();
	mpSelectAll = mpMenu->AddItem("Select All");
	mpMenu->AddSeparator();
	mpMenu->AddItem("Insert", NULL, mpInsertMenu);
	mpMenu->GetItemClickEvent()->Add(this, &CEditorPage::MenuItemClicked);
	mpInsertMenu->GetItemClickEvent()->Add(this, &CEditorPage::MenuItemClicked);
	SetContextMenu(mpMenu);

	PostInit();
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CEditorPage::Class(void)
{
	CFillContainer::Class();

	M_Pointer(mpTextArea);
	U_String(mszFilename);
	M_Pointer(mpMenu);
	M_Pointer(mpCut);
	M_Pointer(mpCopy);
	M_Pointer(mpPaste);
	M_Pointer(mpSelectAll);
	M_Pointer(mpInsertMenu);
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CEditorPage::Free(void)
{
	if (mpMenu.IsNotNull())
	{
		mpMenu->GetItemClickEvent()->Remove(this);
	}
	if (mpInsertMenu.IsNotNull())
	{
		mpInsertMenu->GetItemClickEvent()->Remove(this);
	}
	mszFilename.Kill();
	CFillContainer::Free();
}


//////////////////////////////////////////////////////////////////////////
//
// Replaces the text with the file's, its line endings made line feeds.
// Returns false, keeping the text, if the file can't be read.
//
//////////////////////////////////////////////////////////////////////////
bool CEditorPage::LoadText(const char* szFilename)
{
	CTextFile	cFile;

	cFile.Init();
	if (!cFile.Read(szFilename))
	{
		cFile.Kill();
		return gcLogger.Error2(__METHOD__, " Could not read [", szFilename, "].", NULL);
	}
	cFile.PassifyNewlines();
	mpTextArea->SetText(cFile.Text());
	cFile.Kill();
	mszFilename.Set(szFilename);
	return true;
}


//////////////////////////////////////////////////////////////////////////
//
// Writes the text to the file, replacing it, with line feeds at the ends
// of the lines.  Returns false if the file can't be written.
//
//////////////////////////////////////////////////////////////////////////
bool CEditorPage::SaveText(const char* szFilename)
{
	CTextFile	cFile;
	bool		bResult;

	cFile.Init(mpTextArea->GetText());
	bResult = cFile.Write(szFilename);
	cFile.Kill();
	if (!bResult)
	{
		return gcLogger.Error2(__METHOD__, " Could not write [", szFilename, "].", NULL);
	}
	mszFilename.Set(szFilename);
	return true;
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
Ptr<CTextArea> CEditorPage::GetTextArea(void)
{
	return mpTextArea;
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
char* CEditorPage::GetFilename(void)
{
	return mszFilename.Text();
}


//////////////////////////////////////////////////////////////////////////
//
// An item in the context menu, or its Insert menu, was clicked.  The menu
// has closed and the keys are back in the text area.  An Insert item's
// text is typed at the caret.
//
//////////////////////////////////////////////////////////////////////////
void CEditorPage::MenuItemClicked(CMenuItem* pcItem)
{
	if (&mpCut == pcItem)
	{
		mpTextArea->CutToClipboard();
	}
	else if (&mpCopy == pcItem)
	{
		mpTextArea->CopyToClipboard();
	}
	else if (&mpPaste == pcItem)
	{
		mpTextArea->PasteFromClipboard();
	}
	else if (&mpSelectAll == pcItem)
	{
		mpTextArea->SelectAll();
	}
	else
	{
		mpTextArea->Paste(pcItem->GetText());
	}
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
Ptr<CMenuContainer> CEditorPage::GetMenu(void)
{
	return mpMenu;
}


//////////////////////////////////////////////////////////////////////////
//
// Whether a tab's page is an editor, the Input tab's isn't.
//
//////////////////////////////////////////////////////////////////////////
bool IsEditorPage(Ptr<CComponent> pPage)
{
	if (pPage.IsNull())
	{
		return false;
	}
	return StringCompare(pPage->ClassName(), "CEditorPage") == 0;
}

