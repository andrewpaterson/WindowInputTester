#ifndef __ISOSURFACE_PAGE_H__
#define __ISOSURFACE_PAGE_H__
#include "SupportLib/FontColours.h"
#include "SupportLib/MeshEditor.h"
#include "SupportLib/Scene.h"
#include "SupportLib/Shape.h"
#include "WindowLib/Canvas.h"
#include "WindowLib/Dialog.h"
#include "WindowLib/GridContainer.h"
#include "WindowLib/Label.h"
#include "ProceduralLib/MeshShapeEditor.h"
#include "SupportLib/MeshUnwrap.h"
#include "SupportLib/MeshBake.h"
#include "ProceduralLib/MeshShapeNormals.h"
#include "SceneCanvasDraw.h"


#define ISOSURFACE_SPHERE_RADIUS	1.0f	//Of each sphere dropped.
#define ISOSURFACE_BLEND_RADIUS		0.5f	//How far the smooth union fills the seams between spheres.
#define ISOSURFACE_SMALLEST_CELL	0.05f	//The isosurface octree's cells, smallest
#define ISOSURFACE_LARGEST_CELL		0.8f	//and largest.
#define ISOSURFACE_TEXELS_PER_UNIT	128.0f	//The baked texture's density.
#define ISOSURFACE_CHECKER_SIZE		0.25f	//The checker's cubes, in world units.
#define ISOSURFACE_CLICK_PIXELS		4		//How far the pointer may move between pressing and letting go of the left button for a click.


//An isosurface tab's page, which File's Isosurface makes: a scene of spheres meshed as one isosurface (ProceduralLib's
//CMeshShapeEditor on a CMeshEditor, its normals from the shape by CMeshShapeNormals), unwrapped (CMeshUnwrap) and a solid
//checker baked onto it (CMeshBake), drawn by the scene draw (CSceneCanvasDraw).  Clicking the left button drops a sphere where the pointer is onto the plane of the screen, through what
//the view turns about: at the start that's z = 0, the view looking along -z.  The spheres are joined by a smooth union, so they
//blend where they meet.  Dragging with the left button turns the view, with the middle or right button pans, and the wheel
//zooms.  R switches between Vulkan and the rasteriser, Home puts the view back, and Delete takes the spheres away.
class CIsosurfacePage : public CGridContainer
{
CONSTRUCTABLE(CIsosurfacePage);
DESTRUCTABLE(CIsosurfacePage);
protected:
	Ptr<CLabel>				mpStatus;
	Ptr<CLabel>				mpHelp;
	Ptr<CCanvas>			mpCanvas;
	Ptr<CSceneCanvasDraw>	mpDraw;
	Ptr<CScene>				mpScene;
	Ptr<CMeshEditor>		mpEditor;  //Whose stack meshes mpShape.
	Ptr<CMeshShapeEditor>	mpShapeEditor;
	Ptr<CMeshShapeNormals>	mpShapeNormals;  //Its normals, from the same shape.
	Ptr<CMeshUnwrap>		mpUnwrap;  //Its UVs,
	Ptr<CMeshBake>			mpBake;  //and the checker baked through them.
	Ptr<CShape>				mpShape;  //The spheres so far, NULL for none.
	int						miSpheres;
	bool					mbInScene;  //The editor's instance is in the scene: it's added with the first sphere.
	SInt32Vec2				msLastPointer;  //In the view, at the last movement over it.
	SInt32Vec2				msPressed;  //Where the left button went down.
	bool					mbClick;  //The left button is down and hasn't moved far enough to be a drag.

public:
	void	Init(Ptr<CWindow> pWindow, SDialogLook* psLook, Ptr<CFontColours> pFontColours);
	void	Class(void) override;
	void	Free(void) override;

	void	FocusView(void);
	bool	DropSphere(int32 x, int32 y);
	void	Clear(void);
	int		NumSpheres(void);

	void	MovementAction(SComponentInputEvent* psEvent);
	void	WheelAction(SComponentInputEvent* psEvent);
	void	PressAction(SComponentInputEvent* psEvent);
	void	ReleaseAction(SComponentInputEvent* psEvent);
	void	RendererAction(SComponentInputEvent* psEvent);
	void	HomeAction(SComponentInputEvent* psEvent);
	void	DeleteAction(SComponentInputEvent* psEvent);

protected:
	void	MakeScene(void);
	void	Remesh(void);
};


#endif // __ISOSURFACE_PAGE_H__

