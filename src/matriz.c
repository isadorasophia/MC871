#include "matriz.h"

Matrix *CreateMatrix(int ncols, int nrows)
{
  Matrix *M=(Matrix *)calloc(1,sizeof(Matrix));

  M->ncols = ncols;
  M->nrows = nrows;
  M->tbrow = AllocIntArray(nrows);
  for (int r=0; r < nrows; r++) {
    M->tbrow[r] = r*ncols;
  }
  M->n   = (size_t)ncols*(size_t)nrows;
  M->val = AllocDoubleArray(M->n);

  return(M);
}

Matrix *TransposeMatrix(const Matrix *A)
{
  Matrix *B = NULL;
  int c,r;

  B = CreateMatrix(A->nrows, A->ncols);
  for(r=0; r < B->nrows; r++){
    for(c=0; c < B->ncols; c++){
      B->val[GetMatrixIndex(B,c,r)] = A->val[GetMatrixIndex(A,r,c)];
    }
  }
  return(B);
}

Matrix *MultMatrices(const Matrix *A, const Matrix *B)
{
  Matrix *M = NULL; /* M = alpha A*B + beta M */
  double  sum=0.0;
  int i,j,k;

  if(A->ncols!=B->nrows)
    Error("Cannot multiply matrices","MultMatrices");

  /* Compute multiplication between matrices */

  M = CreateMatrix(B->ncols,A->nrows);

  for (i = 0; i < A->nrows; i++)
	for (j = 0; j < B->ncols; j++){
		for (k = 0; k < A->ncols; k++){
			sum += A->val[k+i*A->ncols] * B->val[j+k*B->ncols];
		}
	M->val[j+i*B->ncols] = sum;
	sum = 0.0;
	}

  return(M);
}

Matrix *RotationMatrix(char axis, float theta)
{
  Matrix *A;
  float cos_theta,sin_theta;

  A         = CreateMatrix(4,4);
  theta     = theta * PI / 180.0;
  cos_theta = cosf(theta);
  sin_theta = sinf(theta);

  switch(axis) {

    case AXIS_X:

      A->val[GetMatrixIndex(A,0,0)] = 1.0;
          A->val[GetMatrixIndex(A,1,0)] = 0.0;
          A->val[GetMatrixIndex(A,2,0)] = 0.0;
          A->val[GetMatrixIndex(A,3,0)] = 0.0;

          A->val[GetMatrixIndex(A,0,1)] = 0.0;
          A->val[GetMatrixIndex(A,1,1)] = cos_theta;
          A->val[GetMatrixIndex(A,2,1)] = -sin_theta;
          A->val[GetMatrixIndex(A,3,1)] = 0.0;

          A->val[GetMatrixIndex(A,0,2)] = 0.0;
          A->val[GetMatrixIndex(A,1,2)] = sin_theta;
          A->val[GetMatrixIndex(A,2,2)] = cos_theta;
          A->val[GetMatrixIndex(A,3,2)] = 0.0;

          A->val[GetMatrixIndex(A,0,3)] = 0.0;
          A->val[GetMatrixIndex(A,1,3)] = 0.0;
          A->val[GetMatrixIndex(A,2,3)] = 0.0;
          A->val[GetMatrixIndex(A,3,3)] = 1.0;

          break;

    case AXIS_Y:

      A->val[GetMatrixIndex(A,0,0)] = cos_theta;
          A->val[GetMatrixIndex(A,1,0)] = 0.0;
          A->val[GetMatrixIndex(A,2,0)] = sin_theta;
          A->val[GetMatrixIndex(A,3,0)] = 0.0;

          A->val[GetMatrixIndex(A,0,1)] = 0.0;
          A->val[GetMatrixIndex(A,1,1)] = 1.0;
          A->val[GetMatrixIndex(A,2,1)] = 0.0;
          A->val[GetMatrixIndex(A,3,1)] = 0.0;

          A->val[GetMatrixIndex(A,0,2)] = -sin_theta;
          A->val[GetMatrixIndex(A,1,2)] = 0.0;
          A->val[GetMatrixIndex(A,2,2)] = cos_theta;
          A->val[GetMatrixIndex(A,3,2)] = 0.0;

          A->val[GetMatrixIndex(A,0,3)] = 0.0;
          A->val[GetMatrixIndex(A,1,3)] = 0.0;
          A->val[GetMatrixIndex(A,2,3)] = 0.0;
          A->val[GetMatrixIndex(A,3,3)] = 1.0;

          break;

    case AXIS_Z:

      A->val[GetMatrixIndex(A,0,0)] = cos_theta;
          A->val[GetMatrixIndex(A,1,0)] = -sin_theta;
          A->val[GetMatrixIndex(A,2,0)] = 0.0;
          A->val[GetMatrixIndex(A,3,0)] = 0.0;

          A->val[GetMatrixIndex(A,0,1)] = sin_theta;
          A->val[GetMatrixIndex(A,1,1)] = cos_theta;
          A->val[GetMatrixIndex(A,2,1)] = 0.0 ;
          A->val[GetMatrixIndex(A,3,1)] = 0.0;

          A->val[GetMatrixIndex(A,0,2)] = 0.0;
          A->val[GetMatrixIndex(A,1,2)] = 0.0;
          A->val[GetMatrixIndex(A,2,2)] = 1.0;
          A->val[GetMatrixIndex(A,3,2)] = 0.0;

          A->val[GetMatrixIndex(A,0,3)] = 0.0;
          A->val[GetMatrixIndex(A,1,3)] = 0.0;
          A->val[GetMatrixIndex(A,2,3)] = 0.0;
          A->val[GetMatrixIndex(A,3,3)] = 1.0;

          break;

    default:
      Error("Invalid option for the axis","RotationMatrix");
  }

  return(A);
}

void DestroyMatrix(Matrix **M) {
  if(M != NULL) {
    Matrix *aux = *M;

    if (aux != NULL) {
      if (aux->val != NULL)
        free(aux->val);
      free(aux->tbrow);
      free(aux);
    }
    *M = NULL;
  }
}
