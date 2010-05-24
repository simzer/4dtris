/**
 * \file  4dt_m3d.c
 * \brief 3D math library.
 */

/*------------------------------------------------------------------------------
   INCLUDE FILES
------------------------------------------------------------------------------*/

#include <math.h>

/*------------------------------------------------------------------------------
   MACROS
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
   TYPES
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
   CONSTANTS
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
   GLOBAL VARIABLES
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
   PROTOTYPES
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
   FUNCTIONS
------------------------------------------------------------------------------*/


/** \brief Calculates the crossProduct of v1 and v2 vector where
    'n' the result vector. */
static void crossProduct(double *n, double v1[3], double v2[3])
{
  // Calculate crossproduct by coordinates.
  n[0] = v1[1] * v2[2] - v2[1] * v1[2];
  n[1] = v1[2] * v2[0] - v2[2] * v1[0];
  n[2] = v1[0] * v2[1] - v2[0] * v1[1];
}

/** \brief Normalise a vector. */
static void normalise(double *v)
{
  // Local variables:
  int i; // loop counter;
  double l; // length of the vector.

  // Calculate length of the vector.
  l = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);

  // For each coordinate
  for (i = 0; i < 3; i++)
  {
    // normalise the coordinate.
    v[i] /= l;
  }
}

/** \brief Calculate normal vector of the plane of v1, v2 vector. */
void m3dCalcNormal(double *n, double v1[3], double v2[3])
{
  // Calculate normal by crossproducting v1 and v2.
  crossProduct(n, v1, v2);
  // Normalise the normal vector.
  normalise(n);
}
