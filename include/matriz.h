#ifndef _MATRIZ_H_
#define _MATRIZ_H_

#include "common.h"

typedef struct doublematrix {
    double *val;                  /* matrix's values */
    int ncols, nrows, *tbrow;     /* number of columns, number of rows,
				   					 and look-up table to speed up index
				   					 access */
    size_t n;                     /* ncols * nrows */
} Matrix, DoubleMatrix;

#define GetMatrixIndex(m,c,r) ((c)+(m)->tbrow[(r)])


Matrix      *CreateMatrix(int ncols, int nrows);
Matrix      *TransposeMatrix(const Matrix *A);
Matrix      *MultMatrices(const Matrix *A, const Matrix *B);
Matrix      *RotationMatrix(char axis, float theta);
void 		 DestroyMatrix(Matrix **M);

#endif