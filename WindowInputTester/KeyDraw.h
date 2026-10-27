#ifndef __KEY_DRAW_H__
#define __KEY_DRAW_H__
#include "StandardLib/Object.h"
#include "StandardLib/Pointer.h"


class CImageDraw;
class CKeyboardCanvasDraw;


//Draws a key's legend onto its face.  The face's edges are in pixels in the keyboard's image.  The image draw's colour
//is the legend colour.
class CKeyDraw : public CObject
{
CONSTRUCTABLE(CKeyDraw);
DESTRUCTABLE(CKeyDraw);
public:
			void	Init(void);
			void	Class(void) override;
			void	Free(void) override;
			bool	Save(CObjectWriter* pcFile) override;
			bool	Load(CObjectReader* pcFile) override;

	virtual void	Draw(CImageDraw* pcDraw, CKeyboardCanvasDraw* pcKeyboardDraw, float64 fLeft, float64 fTop, float64 fRight, float64 fBottom) =0;
};


#endif // __KEY_DRAW_H__
