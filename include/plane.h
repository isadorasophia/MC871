#ifndef _PLANE_H_
#define _PLANE_H_

#include "common.h"

typedef struct _plane {
  Point  pos;    // reference position
  Vector normal; // normal vector which gives its orientation
} Plane;

Plane       *CreatePlane(void);
void         SetPlanePos(Plane *pl, float x, float y, float z);
void         SetPlaneOrient(Plane *pl, float Nx, float Ny, float Nz);
void         DestroyPlane(Plane **pl);

#endif