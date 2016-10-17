#pragma once

/* Implementation of API that enables rotation and 
 * translation of a given 3D image 
 *   - Supports wireframe view from a 3D image 
 * */

#include "ext_image.h"

/* Set properties according to a cubic form */
#define N_FACES 6
#define N_VERT 8

/* Apply custom boolean type */
#define bool int8_t
#define true  1
#define false 0

/* Uncomment to apply debug */
// #define DEBUG 1

Point origin = {0, 0, 0};

Vector zaxis = {0, 0, 1};
Vector yaxis = {0, 1, 0};
Vector xaxis = {1, 0, 0};

Vector isVisible = {0, 0, -1};

/* Regarding visualization of features from model */
Point face[N_FACES] = {{ 1,  0,  0},
                       {-1,  0,  0},
                       { 0,  1,  0},
                       { 0, -1,  0},
                       { 0,  0,  1},
                       { 0,  0, -1}};

Point vert[N_VERT] = {{ 0,  0,  0},
                      { 0,  1,  0},
                      { 1,  1,  0},
                      { 1,  0,  0},
                      { 0,  0,  1},
                      { 0,  1,  1},
                      { 1,  1,  1},
                      { 1,  0,  1}};

/* Return the dot product between two vectors */
float dot_product(Vector x, Vector y) {
    return x.x * y.x + x.y * y.y + x.z * y.z;
}

/* Find diagonal of a vector */
float diagonal(Point size) {
    return sqrt(dot_product(size, size));
}

/* Return cos of the angle between both vectors (NORMALIZED)! */
float find_cos(Vector x, Vector y) {
    return dot_product(x, y);
}

/* Return sin of an agle given its cos */
float find_sin(float cos) {
    return sqrt(1 - cos*cos);
}

/* Normalize a vector */
void normalize(Vector* x) {
    float n = diagonal(*x);

    /* Apply normalization given its size */
    x->x = x->x/n;
    x->y = x->y/n;
    x->z = x->z/n;
}

/* Returns signal of number */
int sign(float p) {
    if (p > 0) {
        return 1;
    } else if (p == 0) {
        return 0;
    } else {
        return -1;
    }
}

/* Perform T(p) */
void translate(Point* t, Point ref) {
    /* Apply translation */
    t->x = t->x + ref.x;
    t->y = t->y + ref.y;
    t->z = t->z + ref.z;
}

/* Perform Ry(thetay) * Rx(thetax) */
void rotatexy(Point* t, float thetax_cos, float thetax_sin, 
                        float thetay_cos, float thetay_sin) {
    /* Rotate in Ry(theta) */
    t->x = thetay_cos * t->x + thetay_sin * t->z;
    t->y = t->y;
    t->z = -thetay_sin * t->x + thetay_cos * t->z;

    /* Rotate in Rx(theta) */
    t->x = t->x;
    t->y = thetax_cos * t->y - thetax_sin * t->z;
    t->z = thetax_sin * t->y - thetax_cos * t->z;
}

/* Find angle between x and y axis based on a vector */
void find_ang(Vector view, float* thetax_cos, float* thetax_sin,
                           float* thetay_cos, float* thetay_sin) {
    /* Normalize view for further operations */
    normalize(&view);

    *thetax_cos = find_cos(view, xaxis);
    *thetax_sin = find_sin(*thetax_cos);
    *thetay_cos = find_cos(view, yaxis);
    *thetay_sin = find_sin(*thetay_cos);
}

/* Apply transformation of rotation in a model from pc, as slide 27 from
 * class 4
 *     pc: {-x/2, -y/2, -z/2}
 *     qc: {D/2, D/2, -D/2} 
 * */
void transform(Point* s, Point pc, Point qc, float thetax_cos, 
               float thetax_sin, float thetay_cos, float thetay_sin) {
    printf("%f %f %f\n", s->x, s->y, s->z);
    printf("x cos, sen: %f %f\ny cos, sen: %f %f\n", thetax_cos, thetax_sin, 
           thetay_cos, thetay_sin);

    translate(s, pc);
    rotatexy(s, thetax_cos, thetax_sin, thetay_cos, thetay_sin);
    translate(s, qc);

    printf("%f %f %f\n", s->x, s->y, s->z);
}

/* Rotate a model given a view (typically, it would be user's mouse) */
MedicalImage* rotate(MedicalImage model, Vector view) {
    float thetax_cos, thetax_sin, thetay_cos, thetay_sin;

    /* Find respective angles based on view vector */
    find_ang(view, &thetax_cos, &thetax_sin, &thetay_cos, &thetay_sin);

    int i, j, k;
    int x = model.nx,
        y = model.ny,
        z = model.nz;

    Point size = {x, y, z};
    float D = diagonal(size);

    /* New reference point */
    Point pc = {-(float)x/2, -(float)y/2, -(float)z/2};
    Point qc = {D/2, D/2, -D/2};

    MedicalImage* rotated;               
    rotated = CreateMedicalImage(D, D, D);

    /* For each point of the model, apply transformation */
    for (i = 0; i < z; i++) {
        for (j = 0; j < y; j++) {
            for (k = 0; k < x; k++) {
                Point t = {k, j, i};

                transform(&t, pc, qc, thetax_cos, thetax_sin, thetay_cos, 
                          thetay_sin);

                rotated->val[(int)t.z][(int)t.y][(int)t.x] = model.val[i][j][k];
            }
        }
    } 

    return rotated;
}


/* Implementation of DDA algorithm, which draws a line from p1 to pn */
void DDA(ColorImage* img, Point p1, Point pn, Cor color) {
    int k, n;
    float Du = 0, Dv = 0, du = 0, dv = 0;

    if (p1.x == pn.x && p1.y == pn.y && p1.z == pn.z) {
        n = 1;
    } else {
        Du = pn.x - p1.x;
        Dv = pn.y - p1.y;

        if (abs(Du) > abs(Dv)) {
            n = abs(Du) + 1;
            du = sign(Du);
            dv = du*Dv/Du;
        } else {
            n = abs(Dv) + 1;
            dv = sign(Dv);
            du = dv*Du/Dv;
        }
    }

    Point p = p1;

    for (k = 0; k < n; k++) {
        img->cor[(int)p.y][(int)p.x] = color;
        p.x += du;
        p.y += dv;
    }
}

ColorImage* draw_wireframe(MedicalImage model, Vector view) {
    int i;
    int x = model.nx,
        y = model.ny,
        z = model.nz;

    Point size = {x, y, z};

    float t, thetax_cos, thetax_sin, thetay_cos, thetay_sin;

    bool draw_face[N_FACES];
    Point rot_face[N_FACES];
    Point rot_vert[N_VERT];

    srand(time(NULL));

    /* Set edge colors */
    Cor color;
    color.val[0] = rand() % 255;
    color.val[1] = rand() % 255;
    color.val[2] = rand() % 255;

    /* Create output */
    float D = diagonal(size);
    ColorImage* output = CreateColorImage(D, D);

    /* Reference for rotation */
    Point pc = {-(float)x/2, -(float)y/2, -(float)z/2};
    Point qc = {D/2, D/2, -D/2};

    /* Find angle based on view vector */
    find_ang(view, &thetax_cos, &thetax_sin, &thetay_cos, &thetay_sin);

    /* Rotate faces according to the new rotation */
    for (i = 0; i < N_FACES; i++) {
        rot_face[i] = face[i];

        rotatexy(&rot_face[i], thetax_cos, thetax_sin, thetay_cos, thetay_sin);
    }

    /* Apply transformation to each of the vertexes */
    for (i = 0; i < N_VERT; i++) {
        rot_vert[i].x = vert[i].x ? x : 0;
        rot_vert[i].y = vert[i].y ? y : 0;
        rot_vert[i].z = vert[i].z ? z : 0;

        transform(&rot_vert[i], pc, qc, thetax_cos, thetax_sin, thetay_cos, 
                  thetay_sin);
    }

    /* Which faces are visible? */
    for (i = 0; i < N_FACES; i++) {
        t = dot_product(rot_face[i], isVisible);

        draw_face[i] = t > 0 ? true : false;
    }

    /* For each visible face, draw its edges */
    for (i = 0; i < N_FACES; i++) {
        if (draw_face[i]) {
            switch(i) {
                case 0:
                    DDA(output, rot_vert[4], rot_vert[5], color);
                    DDA(output, rot_vert[5], rot_vert[6], color);
                    DDA(output, rot_vert[6], rot_vert[7], color);
                    DDA(output, rot_vert[7], rot_vert[4], color);

                case 1:
                    DDA(output, rot_vert[0], rot_vert[1], color);
                    DDA(output, rot_vert[1], rot_vert[2], color);
                    DDA(output, rot_vert[2], rot_vert[3], color);
                    DDA(output, rot_vert[3], rot_vert[0], color);

                case 2:
                    DDA(output, rot_vert[1], rot_vert[5], color);
                    DDA(output, rot_vert[5], rot_vert[6], color);
                    DDA(output, rot_vert[6], rot_vert[2], color);
                    DDA(output, rot_vert[2], rot_vert[1], color);

                case 3:
                    DDA(output, rot_vert[0], rot_vert[4], color);
                    DDA(output, rot_vert[4], rot_vert[7], color);
                    DDA(output, rot_vert[7], rot_vert[3], color);
                    DDA(output, rot_vert[3], rot_vert[0], color);

                case 4:
                    DDA(output, rot_vert[3], rot_vert[2], color);
                    DDA(output, rot_vert[2], rot_vert[6], color);
                    DDA(output, rot_vert[6], rot_vert[7], color);
                    DDA(output, rot_vert[7], rot_vert[3], color);

                case 5:
                    DDA(output, rot_vert[0], rot_vert[1], color);
                    DDA(output, rot_vert[1], rot_vert[5], color);
                    DDA(output, rot_vert[5], rot_vert[4], color);
                    DDA(output, rot_vert[4], rot_vert[0], color);
            }
        }
    }

    return output;
}