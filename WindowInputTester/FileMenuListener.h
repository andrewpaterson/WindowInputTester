#ifndef __FILE_MENU_LISTENER_H__
#define __FILE_MENU_LISTENER_H__
#include "BaseLib/Chars.h"
#include "SupportLib/FontColours.h"
#include "WindowLib/MenuContainer.h"
#include "WindowLib/TabbedContainer.h"
#include "WindowLib/Dialog.h"
#include "WindowLib/FileChooser.h"
#include "EditorPage.h"
#include "ScenePage.h"
#include "VRMLPage.h"


enum EFileMenuDialog
{
	FMD_None,
	FMD_InputTester,
	FMD_NewEditorTab,
	FMD_OpenFile,
	FMD_SaveFile,
};


//The File menu.  Input Tester adds a closable Input tab, or selects it if it's already open, and focusses its page so the
//keys go to it.  New Editor Tab asks for a name with AskTabName and adds a closable editor tab with it, which is
//selected; Cancel, or an empty name, adds nothing.  Open File asks for a file with a CFileChooser and loads it into a new
//closable tab named after the file, which is selected: a scene drawn by Vulkan for a .drg file, a VRML view for a .wrl file
//and an editor for any other.  Save File asks for a file to save the selected editor
//tab's text to, starting on the file it was last loaded from or saved to, and renames the tab after it; it does
//nothing if the selected tab isn't an editor.
//Clicking an item (MenuItemClicked, on the menu bar's item click event) only asks for its dialog; the window's tick
//shows it with ShowDialog.
class CFileMenuListener : public CObject
{
CONSTRUCTABLE(CFileMenuListener);
DESTRUCTABLE(CFileMenuListener);
protected:
	Ptr<CWindow>			mpWindow;
	SDialogLook*			mpsLook;
	Ptr<CFontColours>		mpFontColours;
	Ptr<CTabbedContainer>	mpTabbed;
	Ptr<CMenuItem>			mpInputTester;
	Ptr<CMenuItem>			mpNewEditorTab;
	Ptr<CMenuItem>			mpOpenFile;
	Ptr<CMenuItem>			mpSaveFile;
	Ptr<CImageCel>			mpFolderIcon;
	Ptr<CImageCel>			mpFileIcon;
	Ptr<CImageCel>			mpKeyboardIcon;
	Ptr<CFileChooser>		mpChooser;  //While Load or Save's chooser is shown.
	CChars					mszLastFile;  //The file last opened or saved, empty if none has been.
	int						miNextEditor;  //The number in the next tab's suggested name.
	EFileMenuDialog			meDialog;  //The dialog to show next tick.

public:
	void					Init(Ptr<CWindow> pWindow, SDialogLook* psLook, Ptr<CFontColours> pFontColours, Ptr<CTabbedContainer> pTabbed, Ptr<CMenuContainer> pFileMenu);
	void					Class(void) override;
	void					Free(void) override;

	bool					Save(CObjectWriter* pcFile) override;
	bool					Load(CObjectReader* pcFile) override;

	Ptr<CTabComponent>		AddEditorTab(const char* szName, const char* szText);
	void					MenuItemClicked(CMenuItem* pcItem);
	void					ShowDialog(void);

protected:
	void					OpenInputTester(void);
	void					NewEditorTab(void);
	void					OpenFile(void);
	void					AddFileTab(const char* szFilename, Ptr<CComponent> pPage);
	void					SaveFile(void);
	Ptr<CEditorPage>		GetSelectedEditor(void);
};


#endif // __FILE_MENU_LISTENER_H__

