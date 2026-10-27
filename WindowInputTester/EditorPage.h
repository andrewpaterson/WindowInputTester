#ifndef __EDITOR_PAGE_H__
#define __EDITOR_PAGE_H__
#include "SupportLib/FontColours.h"
#include "WindowLib/FillContainer.h"
#include "WindowLib/TextArea.h"
#include "WindowLib/WindowTheme.h"


//An editor tab's page: an editable, word wrapped text area in a scroll pane over the editor's colour.  It remembers
//the file it was last loaded from or saved to.
class CEditorPage : public CFillContainer
{
CONSTRUCTABLE(CEditorPage);
DESTRUCTABLE(CEditorPage);
protected:
	Ptr<CTextArea>	mpTextArea;
	CChars			mszFilename;  //Empty if it hasn't been loaded or saved.

public:
	void			Init(Ptr<CWindow> pWindow, Ptr<CWindowTheme> pTheme, Ptr<CFontColours> pFontColours, const char* szText);
	void			Class(void) override;
	void			Free(void) override;

	bool			LoadText(const char* szFilename);
	bool			SaveText(const char* szFilename);
	char*			GetFilename(void);
	Ptr<CTextArea>	GetTextArea(void);
};


bool IsEditorPage(Ptr<CComponent> pPage);


#endif // __EDITOR_PAGE_H__

