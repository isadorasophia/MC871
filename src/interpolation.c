#include "interpolation.h"

MedicalImage *Interp(MedicalImage *img, float sx, float sy, float sz){
	MedicalImage	*ximg, *yimg, *zimg;  
    int 			nx,ny,nz;
  
	if (img->nz == 1)
    	Error("Use iftInterp2D for 2D images","Interp");
  	if ((sx <= 0.0)||(sy <= 0.0)||(sz <= 0.0))
    	Error("Invalid scale factors","Interp");
  
  	nx= ROUND(fabs(sx * img->nx));
  	ny= ROUND(fabs(sy * img->ny));
  	nz= ROUND(fabs(sz * img->nz));
  
  	if (sx != 1.0 ) {
    
    	/* Interpolate along x */
	  
    	ximg = CreateMedicalImage(nx,img->ny, img->nz);
    	ximg->dx = img->dx/sx;
    	ximg->dy = img->dy;
    	ximg->dz = img->dz;
    
    
    	for (int z = 0; z < ximg->nz; z++){		  
      		Voxel u, v, w;
      		u.z = w.z = v.z = z;
      		for (v.y = 0; v.y < ximg->ny; v.y++){
				u.y = w.y = v.y;
				for (v.x = 0; v.x < ximg->nx; v.x++){   
	  				u.x  = (int)(v.x/sx);
	  				float dx   = (v.x/sx) - u.x; 
	  				w.x  = ((u.x+1)==img->nx)?u.x:u.x+1;
	  				ximg->val[v.z][v.y][v.x] = (int)(img->val[u.z][u.y][u.x]*(1.0-dx)+img->val[w.z][w.y][w.x]*dx);
				}
      		}
    	}
  	}else{
    	ximg = CopyMedicalImage(img);
  	}
  
  	if (sy != 1.0) { 
    
    	/* Interpolate along y */
    
    	yimg = CreateMedicalImage(nx,ny, img->nz);
    	yimg->dx = ximg->dx;
    	yimg->dy = ximg->dy/sy;
    	yimg->dz = ximg->dz;
    
    	for (int z = 0; z < yimg->nz; z++){
      		Voxel u,w,v;
      		u.z = w.z = v.z = z;
      		for (v.x = 0; v.x < yimg->nx; v.x++){
				u.x = w.x = v.x;
				for (v.y = 0; v.y < yimg->ny; v.y++){  
	  				u.y  = (int)(v.y/sy);
	  				float dy   = (v.y/sy) - u.y; 
	  				w.y  = ((u.y+1)==ximg->ny)?u.y:u.y+1;
	  				yimg->val[v.z][v.y][v.x] = (int)(ximg->val[u.z][u.y][u.x]*(1.0-dy)+ximg->val[w.z][w.y][w.x]*dy);
				}
      		}
    	}
  	} else {
    	yimg = CopyMedicalImage(ximg);
  	}
  
  	DestroyMedicalImage(&ximg);
  
  	if (sz != 1.0) {
    
    	/* Interpolate along z */
    
    	zimg = CreateMedicalImage(nx,ny,nz);
    	zimg->dx = yimg->dx;
    	zimg->dy = yimg->dy;
    	zimg->dz = yimg->dz/sz;
    
    	for (int y = 0; y < zimg->ny; y++){
      		Voxel u,v,w;
      		u.y = w.y = v.y = y;
      		for (v.x = 0; v.x < zimg->nx; v.x++){
				u.x = w.x = v.x;
				for (v.z = 0; v.z < zimg->nz; v.z++){  
					u.z  = (int)(v.z/sz);
	  				float dz   = (v.z/sz) - u.z; 
	  				w.z  = ((u.z+1)==yimg->nz)?u.z:u.z+1;
	  				zimg->val[v.z][v.y][v.x] = (int)(yimg->val[u.z][u.y][u.x]*(1.0-dz)+yimg->val[w.z][w.y][w.x]*dz);
				}
      		}
    	}
    
  	} else { 
    	zimg = CopyMedicalImage(yimg);
  	}
  
  	DestroyMedicalImage(&yimg);
  
  	return(zimg);
}

int ImageValueAtPoint(const MedicalImage *img, Point P){
  Voxel u[8];
  int i, value;
  float dx=1.0,dy=1.0,dz=1.0, val[6];


  if ((int)(P.x+1.0)==img->nx) dx = 0.0;
  if ((int)(P.y+1.0)==img->ny) dy = 0.0;
  if ((int)(P.z+1.0)==img->nz) dz = 0.0;

  u[0].x = (int)P.x;      u[0].y = (int)P.y;       u[0].z = (int)P.z;
  u[1].x = (int)(P.x+dx); u[1].y = (int)P.y;       u[1].z = (int)P.z;
  u[2].x = (int)P.x;      u[2].y = (int)(P.y+dy);  u[2].z = (int)P.z;
  u[3].x = (int)(P.x+dx); u[3].y = (int)(P.y+dy);  u[3].z = (int)P.z;
  u[4].x = (int)P.x;      u[4].y = (int)P.y;       u[4].z = (int)(P.z+dz);
  u[5].x = (int)(P.x+dx); u[5].y = (int)P.y;       u[5].z = (int)(P.z+dz);
  u[6].x = (int)P.x;      u[6].y = (int)(P.y+dy);  u[6].z = (int)(P.z+dz);
  u[7].x = (int)(P.x+dx); u[7].y = (int)(P.y+dy);  u[7].z = (int)(P.z+dz);

  for (i=0; i < 8; i++) {
    if (!ValidVoxel(img,u[i])){    
      u[0].x = ROUND(P.x);
      u[0].y = ROUND(P.y);
      u[0].z = ROUND(P.z);
      return(img->val[u[0].z][u[0].y][u[0].x]);
    }

  }

  val[0] =(float)img->val[u[1].z][u[1].y][u[1].x]*(P.x-u[0].x)+(float)img->val[u[0].z][u[0].y][u[0].x]*(u[1].x-P.x);
  val[1] =(float)img->val[u[3].z][u[3].y][u[3].x]*(P.x-u[2].x)+(float)img->val[u[2].z][u[2].y][u[2].x]*(u[3].x-P.x);  
  val[2] =(float)img->val[u[5].z][u[5].y][u[5].x]*(P.x-u[4].x)+(float)img->val[u[4].z][u[4].y][u[4].x]*(u[5].x-P.x);  
  val[3] =(float)img->val[u[7].z][u[7].y][u[7].x]*(P.x-u[6].x)+(float)img->val[u[6].z][u[6].y][u[6].x]*(u[7].x-P.x);  
  val[4] = val[1]*(P.y-u[0].y) + val[0]*(u[2].y-P.y);
  val[5] = val[3]*(P.y-u[0].y) + val[2]*(u[2].y-P.y);
  value  = (int)(val[5]*(P.z-u[0].z) + val[4]*(u[4].z-P.z) + 0.5);

  return(value);
}