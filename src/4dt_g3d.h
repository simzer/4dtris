/**
 *  \file  4dt_g3d.h
 *  \brief Header for 3D OpenGL scene rendering modul.
 */

#ifndef _4DT_G3D_H_
#define _4DT_G3D_H_

#ifndef _4DT_M3D_H_
#error "please include 4dt_m3d.h first"
#endif
/*------------------------------------------------------------------------------
   MACROS
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
   TYPES
------------------------------------------------------------------------------*/

typedef enum
{
  eG3dFill = 0,
  eG3dFillAndWire = 1,
  eG3dWire = 2,
  eG3dWireTube = 3
}
tG3dFillMode;


/*------------------------------------------------------------------------------
   DECLARATIONS
------------------------------------------------------------------------------*/

extern void g3dInit(void);
extern void g3dDrawPoly(tM3dVector points[4],
                        float color[4],
                        tG3dFillMode mode,
                        int sideVisible[4]);
extern void g3dDrawLine(tM3dVector point0,
                        tM3dVector point1,
                        float color0[4],
                        float color1[4],
                        float linewidth);
extern void g3dDrawRectangle(float x0, float y0, float x1, float y1,
                      float color1[4], float color2[4]);
extern void g3dBeginDraw(int x, int y, int z, int picnum, tScnViewMode viewMode);
extern void g3dEndDrawPic(void);
extern void g3dEndDraw(void);
extern void g3dResize(int width, int height);

#endif /* _4DT_G3D_H_ */
