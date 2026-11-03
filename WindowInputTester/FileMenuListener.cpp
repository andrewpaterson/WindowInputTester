#include "BaseLib/FileUtil.h"
#include "StandardLib/Objects.h"
#include "StandardLib/ObjectReader.h"
#include "StandardLib/ObjectWriter.h"
#include "WindowLib/FileChooser.h"
#include "WindowLib/JewelImageGenerator.h"
#include "WindowLib/JewelColours.h"
#include "TabNameListener.h"
#include "FileMenuListener.h"


//Open File's Files of Type.
static SFileChooserType gasTextFileTypes[] =
{
	{ "Text Files (*.txt)", "txt" },
	{ "All Files", NULL },
};


//////////////////////////////////////////////////////////////////////////
//
// Adds the menu's items to pFileMenu.
//
//////////////////////////////////////////////////////////////////////////
void CFileMenuListener::Init(Ptr<CWindow> pWindow, SDialogLook* psLook, Ptr<CFontColours> pFontColours, Ptr<CTabbedContainer> pTabbed, Ptr<CMenuContainer> pFileMenu)
{
	CColourFormatHelper		cFormat;

	PreInit();

	mpWindow = pWindow;
	mpsLook = psLook;
	mpFontColours = pFontColours;
	mpTabbed = pTabbed;
	mszLastFile.Init();
	miNextEditor = 2;
	meDialog = FMD_None;

	pWindow->GetColourFormat(&cFormat);
	mpFolderIcon = CJewelImageGenerator::CreateFolder(JEWEL_ICON, &cFormat);
	mpFileIcon = CJewelImageGenerator::CreateFile(JEWEL_ICON, &cFormat);

	mpNewEditorTab = pFileMenu->AddItem("New...");
	mpOpenFile = pFileMenu->AddItem("Load...", mpFileIcon);
	mpSaveFile = pFileMenu->AddItem("Save...");

	PostInit();
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CFileMenuListener::Class(void)
{
	M_Pointer(mpWindow);
	U_Pointer(mpsLook);
	M_Pointer(mpFontColours);
	M_Pointer(mpTabbed);
	M_Pointer(mpNewEditorTab);
	M_Pointer(mpOpenFile);
	M_Pointer(mpSaveFile);
	M_Pointer(mpFolderIcon);
	M_Pointer(mpFileIcon);
	M_Pointer(mpChooser);
	U_String(mszLastFile);
	U_Int32(miNextEditor);
	U_Enum(meDialog);
}


//////////////////////////////////////////////////////////////////////////
//
// A listener isn't saved.
//
//////////////////////////////////////////////////////////////////////////
bool CFileMenuListener::Save(CObjectWriter* pcFile)
{
	return false;
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
bool CFileMenuListener::Load(CObjectReader* pcFile)
{
	return false;
}



//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CFileMenuListener::Free(void)
{
	//An object's memory is freed without its destructor running so the listener takes itself off the menu bar.
	RemoveFromListenerCalls();
	mszLastFile.Kill();
}


//////////////////////////////////////////////////////////////////////////
//
// A closable editor tab holding szText.  It isn't selected.
//
//////////////////////////////////////////////////////////////////////////
Ptr<CTabComponent> CFileMenuListener::AddEditorTab(const char* szName, const char* szText)
{
	return mpTabbed->AddTab(szName, OMalloc<CEditorPage>(mpWindow, mpsLook->pcTheme, mpFontColours, szText), NULL, true);
}


//////////////////////////////////////////////////////////////////////////
//
// Asks for the item's dialog, shown next tick.
//
//////////////////////////////////////////////////////////////////////////
void CFileMenuListener::MenuItemClicked(CUnknown* pcSource, void* pvContext)
{
	if (&mpNewEditorTab == pcSource)
	{
		meDialog = FMD_NewEditorTab;
	}
	else if (&mpOpenFile == pcSource)
	{
		meDialog = FMD_OpenFile;
	}
	else if (&mpSaveFile == pcSource)
	{
		meDialog = FMD_SaveFile;
	}
}


//////////////////////////////////////////////////////////////////////////
//
// Called by the window's tick.  Shows the dialog asked for, if any.
//
//////////////////////////////////////////////////////////////////////////
void CFileMenuListener::ShowDialog(void)
{
	EFileMenuDialog	eDialog;

	eDialog = meDialog;
	meDialog = FMD_None;
	if (eDialog == FMD_NewEditorTab)
	{
		NewEditorTab();
	}
	else if (eDialog == FMD_OpenFile)
	{
		OpenFile();
	}
	else if (eDialog == FMD_SaveFile)
	{
		SaveFile();
	}
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CFileMenuListener::NewEditorTab(void)
{
	CChars	szSuggested;
	CChars	szName;

	szSuggested.Init("Unnamed");
	szName.Init();
	if (AskTabName(mpWindow, mpsLook, "New Editor Tab", szSuggested.Text(), &szName))
	{
		mpTabbed->Select(AddEditorTab(szName.Text(), ""));
		miNextEditor++;
	}
	szName.Kill();
	szSuggested.Kill();
}


//////////////////////////////////////////////////////////////////////////
//
// The chooser starts on the file last opened or saved.  A file that can't
// be read adds no tab.
//
//////////////////////////////////////////////////////////////////////////
void CFileMenuListener::OpenFile(void)
{
	CFileUtil			cFileUtil;
	Ptr<CEditorPage>	pEditor;
	CChars				szName;
	char*				szFilename;

	mpChooser = OMalloc<CFileChooser>(mpWindow, mpsLook, "Open File", false, mszLastFile.Text(), gasTextFileTypes, sizeof(gasTextFileTypes) / sizeof(SFileChooserType), mpFolderIcon, mpFileIcon);
	if (mpChooser->Show())
	{
		szFilename = mpChooser->GetChosen();
		mszLastFile.Set(szFilename);

		pEditor = OMalloc<CEditorPage>(mpWindow, mpsLook->pcTheme, mpFontColours, "");
		if (pEditor->LoadText(szFilename))
		{
			szName.Init();
			cFileUtil.GetFileName(&szName, szFilename);
			mpTabbed->Select(mpTabbed->AddTab(szName.Text(), pEditor, NULL, true));
			szName.Kill();
		}
	}
	mpChooser = NULL;
}


//////////////////////////////////////////////////////////////////////////
//
// The chooser starts on the editor's file, or if it has none, on a file
// named after the tab in the folder of the file last opened or saved.
//
//////////////////////////////////////////////////////////////////////////
void CFileMenuListener::SaveFile(void)
{
	CFileUtil			cFileUtil;
	Ptr<CEditorPage>	pEditor;
	CChars				szStart;
	CChars				szName;
	CChars				szDirectory;
	char*				szFilename;

	pEditor = GetSelectedEditor();
	if (pEditor.IsNull())
	{
		return;
	}

	szStart.Init(pEditor->GetFilename());
	if (szStart.Empty())
	{
		szName.Init();
		szDirectory.Init();
		cFileUtil.SplitPath(mszLastFile.Text(), &szName, &szDirectory);
		szStart.Append(&szDirectory);
		cFileUtil.AppendToPath(&szStart, mpTabbed->GetSelected()->GetText());
		szDirectory.Kill();
		szName.Kill();
	}

	mpChooser = OMalloc<CFileChooser>(mpWindow, mpsLook, "Save File", true, szStart.Text(), gasTextFileTypes, sizeof(gasTextFileTypes) / sizeof(SFileChooserType), mpFolderIcon, mpFileIcon);
	if (mpChooser->Show())
	{
		szFilename = mpChooser->GetChosen();
		if (pEditor->SaveText(szFilename))
		{
			mszLastFile.Set(szFilename);
			szName.Init();
			cFileUtil.GetFileName(&szName, szFilename);
			mpTabbed->GetSelected()->SetText(szName.Text());
			szName.Kill();
		}
	}
	mpChooser = NULL;
	szStart.Kill();
}


//////////////////////////////////////////////////////////////////////////
//
// The selected tab's page if it's an editor, otherwise NULL.
//
//////////////////////////////////////////////////////////////////////////
Ptr<CEditorPage> CFileMenuListener::GetSelectedEditor(void)
{
	Ptr<CTabComponent>	pSelected;
	Ptr<CComponent>		pPage;
	size				i;

	pSelected = mpTabbed->GetSelected();
	for (i = 0; i < mpTabbed->NumTabs(); i++)
	{
		if (mpTabbed->GetTab(i) == pSelected)
		{
			pPage = mpTabbed->GetPage(i);
			if (IsEditorPage(pPage))
			{
				return Ptr<CEditorPage>(&pPage);
			}
		}
	}
	return NULL;
}

