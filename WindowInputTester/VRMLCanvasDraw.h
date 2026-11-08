#ifndef __VRML_CANVAS_DRAW_H__
#define __VRML_CANVAS_DRAW_H__
#include "RasterLib/Rasteriser.h"
#include "SupportLib/Image.h"
#include "SupportLib/Mesh.h"
#include "WindowLib/CanvasDraw.h"
#include "WindowLib/ImageCanvasDraw.h"
#include "WindowLib/WindowTheme.h"


//Draws a mesh with RasterLib's rasteriser into a cel the size of the canvas, which an image draw puts on the canvas.
//The mesh is only drawn again when the view or the canvas's size changes, as it can take a large part of a second.
//A new size keeps the view.
class CVRMLCanvasDraw : public CCanvasDraw
{
CONSTRUCTABLE(CVRMLCanvasDraw);
DESTRUCTABLE(CVRMLCanvasDraw);
protected:
	Ptr<CImageCanvasDraw>	mpCelDraw;
	Ptr<CImage>				mpImage;  //What the rasteriser draws on.
	CRasterMesh				mcMesh;
	CRasterCamera			mcCamera;
	CRasteriser				mcRasteriser;
	CRasterTarget			mcTarget;
	uint32					muiBackground;  //0xAARRGGBB.
	SInt32Vec2				msSize;  //The canvas size mpImage and mcTarget were made for, -1, -1 for none yet.
	bool					mbDirty;

public:
	void	Init(Ptr<CWindowTheme> pTheme);
	void	Class(void) override;
	void	Free(void) override;
	bool	Save(CObjectWriter* pcFile) override;
	bool	Load(CObjectReader* pcFile) override;

	bool	Draw(Ptr<CCanvas> pCanvas) override;

	void	SetMesh(CMesh* pcMesh);
	size	NumTriangles(void);
	void	Drag(int32 iLastX, int32 iLastY, int32 iX, int32 iY);
	void	Pan(int32 iRight, int32 iDown);
	void	PanStep(int32 iRight, int32 iUp);
	void	Zoom(float32 fFactor);
	bool	LookAtScreen(int32 iX, int32 iY);
};


#endif // __VRML_CANVAS_DRAW_H__

