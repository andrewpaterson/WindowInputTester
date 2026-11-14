#ifndef __INPUT_PAGE_H__
#define __INPUT_PAGE_H__
#include "WindowLib/GridContainer.h"
#include "WindowLib/WindowTheme.h"


//The Input tab's page, which File's Input Tester makes: the mouse and a blank cell on top, each half the width, over the
//keyboard spanning both.  The rows are each half the height.  The page takes focus when clicked so the keys have somewhere
//to go.
//
//The mouse and keyboard views draw every paint, so they show the devices as they are: the keyboard and the mouse's buttons
//are read as they're drawn.  The wheel's rolls and the mouse's movement are only events, which go from the focussed
//component (the one under the pointer for movement) up to the page, which shows them on the mouse and passes them on.
//Closing the tab lets all of it go.
class CInputPage : public CGridContainer
{
CONSTRUCTABLE(CInputPage);
DESTRUCTABLE(CInputPage);
public:
	void	Init(Ptr<CWindow> pWindow, Ptr<CWindowTheme> pTheme);
	void	Class(void) override;
	void	Free(void) override;
};


bool IsInputPage(Ptr<CComponent> pPage);


#endif // __INPUT_PAGE_H__

