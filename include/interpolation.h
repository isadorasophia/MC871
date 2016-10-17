#ifndef _INTERPOLATION_H_
#define _INTERPOLATION_H_

#include "common.h"
#include "image.h"

MedicalImage   *Interp(MedicalImage *img, float sx, float sy, float sz);
int 			ImageValueAtPoint(const MedicalImage *img, Point P);

#endif