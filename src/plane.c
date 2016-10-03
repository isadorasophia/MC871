#include "plane.h"

Plane *CreatePlane()
{
  Plane *pl=(Plane *)calloc(1,sizeof(Plane));

  SetPlanePos(pl,0,0,0);
  SetPlaneOrient(pl,0.0,0.0,1.0);

  return(pl);
}

void DestroyPlane(Plane **pl)
{
  Plane *aux=*pl;

  if (aux != NULL){
    free(aux);
    *pl = NULL;
  }
}

void SetPlanePos(Plane *pl, float x, float y, float z)
{
  pl->pos.x = x;
  pl->pos.y = y;
  pl->pos.z = z;
}

void SetPlaneOrient(Plane *pl, float Nx, float Ny, float Nz)
{
  float m = sqrtf(Nx*Nx + Ny*Ny + Nz*Nz); 

  if (m==0.0)
    Error("There is no orientation","SetPlaneOrient");

  pl->normal.x = Nx/m;
  pl->normal.y = Ny/m;
  pl->normal.z = Nz/m;
}