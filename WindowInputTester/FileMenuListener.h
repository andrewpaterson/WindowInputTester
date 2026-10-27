#ifndef __FILE_MENU_LISTENER_H__
#define __FILE_MENU_LISTENER_H__
#include "BaseLib/Chars.h"
#include "SupportLib/FontColours.h"
#include "WindowLib/MenuContainer.h"
#include "WindowLib/MenuListener.h"
#include "WindowLib/TabbedContainer.h"
#include "WindowLib/Dialog.h"
#include "EditorPage.h"


enum EFileMenuDialog
{
	FMD_None,
	FMD_NewEditorTab,
	FMD_OpenFile,
	FMD_SaveFile,
};


//The File menu.  New Editor Tab asks for a name with AskTabName and adds a closable editor tab with it, which is
//selected; Cancel, or an empty name, adds nothing.  Open File asks for a text file with a CFileChooser and loads it
//into a new editor tab named after the file, which is selected.  Save File asks for a file to save the selected editor
//tab's text to, starting on the file it was last loaded from or saved to, and renames the tab after it; it does
//nothing if the selected tab isn't an editor.
//Clicking an item only asks for its dialog; the window's tick shows it with ShowDialog.
class CFileMenuListener : public CMenuListener
{
CONSTRUCTABLE(CFileMenuListener);
protected:
	Ptr<CWindow>			mpWindow;
	SDialogLook*			mpsLook;
	Ptr<CFontColours>		mpFontColours;
	Ptr<CTabbedContainer>	mpTabbed;
	Ptr<CMenuItem>			mpNewEditorTab;
	Ptr<CMenuItem>			mpOpenFile;
	Ptr<CMenuItem>			mpSaveFile;
	Ptr<CImageCel>			mpFolderIcon;
	Ptr<CImageCel>			mpFileIcon;
	CChars					mszLastFile;  //The file last opened or saved, empty if none has been.
	int						miNextEditor;  //The number in the next tab's suggested name.
	EFileMenuDialog			meDialog;  //The dialog to show next tick.

public:
	void					Init(Ptr<CWindow> pWindow, SDialogLook* psLook, Ptr<CFontColours> pFontColours, Ptr<CTabbedContainer> pTabbed, Ptr<CMenuContainer> pFileMenu);
	void					Kill(void);

	Ptr<CTabComponent>		AddEditorTab(const char* szName, const char* szText);
	void					MenuItemClicked(CUnknown* pcSource, void* pvContext) override;
	void					ShowDialog(void);

protected:
	void					NewEditorTab(void);
	void					OpenFile(void);
	void					SaveFile(void);
	Ptr<CEditorPage>		GetSelectedEditor(void);
};


#endif // __FILE_MENU_LISTENER_H__

