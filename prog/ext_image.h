#pragma once

#include "mc920.h"

/* Coordenates */
typedef enum { XY, XZ, ZY } Axis;
typedef enum { IN = 1, OUT = 2 } Orientation;
typedef enum { R = 0, G = 1, B = 2 } RGB;

typedef enum { AXIAL = 1, CORONAL, SAGITAL } Cut;

#define H 255

#define MODEL_MAX 4095 // default max value of vortex from models

#define CLAMP_COLOR(p) (p > H ? H : p)
#define CLAMP_NEG(p) (p < 0 ? 0 : p)
#define CLAMP_PERC(p) (p > 0.999 ? 0.999 : p)

// #define MAX(p, q) (p > q ? p : q)

#define ERROR -1
#define NONE  -1
#define OK     0

/* Torna uma imagem 3D em 2D, dado:
 *   a: eixos do plano (XY, XZ ou ZY)
 *   o: orientacao da imagem (IN ou OUT)
 *
 *   x e y: dimensoes da imagem
 *   origin: coordenada de origem
 * */
 /* MedicalImage: [z][y][x] */
int to2d(MedicalImage* model, GrayImage** img, Axis a, Orientation o, 
         int x, int y, int origin) {
    int i, j;

    for (i = 0; i < y; i++) {
        for (j = 0; j < x; j++) {
            /* Set coordinates accordingly */
            if (a == XY) {
                switch (o) {
                    // i := y := ny
                    // j := x := nx
                    case IN:
                        (*img)->val[i][j] = model->val[origin][i][j];

                        break;

                    // i := y := nx
                    // j := x := ny
                    case OUT:
                        (*img)->val[i][j] = model->val[origin][j][i];

                        break;
                }
            } else if (a == XZ) {
                switch (o) {
                    // i := y := nz
                    // j := x := nx
                    case IN:
                        (*img)->val[i][j] = model->val[y - 1 - i][origin][j];

                        break;

                    // i := y := nx
                    // j := x := nz
                    case OUT:
                        (*img)->val[i][j] = model->val[y - 1 - j][origin][i];

                        break;
                }
            } else if (a == ZY) {
                switch (o) {
                    // i := y := ny
                    // j := x := nz
                    case IN:
                        (*img)->val[i][j] = model->val[y - 1 - j][i][origin];

                        break;

                    // y := nz
                    // x := ny
                    case OUT:
                        (*img)->val[i][j] = model->val[y - 1 - i][j][origin];

                        break;
                }
            }
        }
    }

    return OK;
}

/* Make a medical cut (AXIAL, CORONAL or SAGITAL) according to cord */
int to2d_cut(MedicalImage* model, GrayImage** img, Cut type, int cord) {
    /* Get cut type */
    if (type == AXIAL) {
        cord = (cord == NONE) ? model->nz/2 : cord;

        if (cord > model->nz) {
            return ERROR;
        }

        to2d(model, img, XY, IN, (*img)->nx, (*img)->ny, cord);

    } else if (type == CORONAL) {
        cord = (cord == NONE) ? model->ny/2 : cord;

        if (cord > model->ny) {
            return ERROR;
        }

        to2d(model, img, XZ, IN, (*img)->nx, (*img)->ny, cord);

    } else if (type == SAGITAL) {
        cord = (cord == NONE) ? model->nx/2 : cord;

        if (cord > model->nx) {
            return ERROR;
        }

        to2d(model, img, ZY, OUT, (*img)->nx, (*img)->ny, cord);

    } else {
        return ERROR;
    }

    return OK;
}
 
/* Altera o brilho e contraste da imagem, dado a porcentagem (0 a 100%) 
 *    p: porcentagem de brilho e contraste (0 a 1)
 * */
int BC(GrayImage* img, int B, int C) {
    /* fix values */
    int i, j;

    int k2 = H,
        k1 = 0,
        I1, I2;

    double B_ = (100 - B) * 0.01 * H,
          C_ = C * 0.01 * H;

    I2 = CLAMP_COLOR((2 * B_ + C_)/2);
    I1 = CLAMP_COLOR((2 * B_ - C_)/2);

    for (i = 0; i < img->ny; i++) {
        for (j = 0; j < img->nx; j++) {
            int I = img->val[i][j];

            if (I < I1) {
                img->val[i][j] = (k1 == 0) ? 1 : k1;
            } else if (I >= I2) {
                img->val[i][j] = k2;
            } else {    
                img->val[i][j] = (I - I1) * (k2 - k1)/(I2 - I1) + k1; 
            }
        }
    }

    return OK;
}

/* Retorna o pixel de itensidade na imagem */
int max_pixel(GrayImage* img) {
    int max = 0;
    int i, j;

    for (i = 0; i < img->ny; i++) {
        for (j = 0; j < img->nx; j++) {
            if (img->val[i][j] > max) {
                max = img->val[i][j];
            }
        }
    }

    return max;
}

/* Aplica uma escala no espectro de cor da imagem, de modo ao maior valor
 * ser o correspondente a "old_max"
 *   - Se "old_max" == NONE, aplica a escala no pixel de maior valor 
 *   da imagem 
 * */
void scale_colorspace(GrayImage* img, int old_max, int new_max) {
    int i, j;

    /* If the scale is optional */
    if (old_max == NONE) {
        old_max = max_pixel(img);
    }

    /* Apply scale! */
    for (i = 0; i < img->ny; i++) {
        for (j = 0; j < img->nx; j++) {
            img->val[i][j] = (double)img->val[i][j]/old_max * new_max;
        }
    }
}

/* Recebe uma imagem distribuída em labels I(p) E {0, 1, ... c} e retorna
   sua distribuição em cores RGB */
int color(GrayImage* img, ColorImage* c_img) {
    int i, j;

    double V;
    double R_, G_, B_;

    for (i = 0; i < img->ny; i++) {
        for (j = 0; j < img->nx; j++) {
            V = img->val[i][j];

            /* If it is not background */
            if (V != 0) {
                R_ = MAX(0, (3 - abs(V - 4) - abs(V - 5))/2) * H;
                G_ = MAX(0, (4 - abs(V - 2) - abs(V - 4))/2) * H;
                B_ = MAX(0, (3 - abs(V - 1) - abs(V - 2))/2) * H;

                c_img->cor[i][j].val[R] = R_;
                c_img->cor[i][j].val[G] = G_;
                c_img->cor[i][j].val[B] = B_;
            
            } else {
                c_img->cor[i][j].val[R] = 0;
                c_img->cor[i][j].val[G] = 0;
                c_img->cor[i][j].val[B] = 0;
            }
        }
    }

    return OK;
}

/* Convert from a rgb space to YCgCo */
void rgb2ycc(Cor cor, double* Y_, double* Cg, double* Co) {
    double R_ = (double)cor.val[R]/H,
          G_ = (double)cor.val[G]/H,
          B_ = (double)cor.val[B]/H; 

    *Y_ =  0.25 * R_ + 0.5 * G_ + 0.25 * B_;
    *Cg = -0.25 * R_ + 0.5 * G_ - 0.25 * B_;
    *Co =  0.5  * R_            - 0.5  * B_;
}

/* Convert from a YCgCo space to a rgb space */
void ycc2rgb(double Y_, double Cg, double Co, Cor* cor) {
    double R_ = Y_ - Cg + Co,
          G_ = Y_ + Cg,
          B_ = Y_ - Cg - Co;

    cor->val[R] = CLAMP_COLOR(R_ * H);
    cor->val[G] = CLAMP_COLOR(G_ * H);
    cor->val[B] = CLAMP_COLOR(B_ * H);
}

void apply_mask(GrayImage* img, ColorImage* c_img) {
    int y = img->ny;
    int x = img->nx;

    int i, j;
    double Cg, Co, Y_;

    for (i = 0; i < y; i++) {
        for (j = 0; j < x; j++) {
            rgb2ycc(c_img->cor[i][j], &Y_, &Cg, &Co);

            double lum = (double)img->val[i][j]/H;

            Y_  = lum;

            Cg *= lum;
            Co *= lum;

            ycc2rgb(Y_, Cg, Co, &(c_img->cor[i][j]));
        }
    }
}
