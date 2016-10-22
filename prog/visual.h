#pragma once

#include "ext_image.h"
#include "wireframe.h"

Point c_face[N_FACES] = {{ 1,   0.5,  0.5},
                         { 0,   0.5,  0.5},
                         { 0.5,   1,  0.5},
                         { 0.5,   0,  0.5},
                         { 0.5, 0.5,    1},
                         { 0.5, 0.5,    0}};

/* Return product one by one of two vectors */
Point product_1b1(Point a, Point b) {
    Point res;

    res.x = a.x * b.x;
    res.y = a.y * b.y;
    res.z = a.z * b.z;

    return res;
}

Point add_points(Point a, Point b) {
    Point res = {a.x + b.x, a.y + b.y, a.z + b.z};

    return res;
}

Point sub_points(Point a, Point b) {
    Point res = {a.x - b.x, a.y - b.y, a.z - b.z};

    return res;
}

Point scalar_mul(float l, Point a) {
    Point res = {a.x*l, a.y*l, a.z*l};

    return res;
}

/* Check if point p is within limits of restraints, given by r */
bool within_limits(Point p, Point r) {
    if (p.x < r.x && p.y < r.y && p.z < r.z &&
        p.x >= 0  && p.y >= 0  && p.z >= 0)
        return true;
    else
        return false;
}

void rotatex_i(Point* t, float cos, float sin) {
    float x = t->x, 
          y = t->y, 
          z = t->z;

    t->x = x;
    t->y =  cos * y + sin * z;
    t->z = -sin * y + cos * z;
}

void rotatey_i(Point* t, float cos, float sin) {
    float x = t->x, 
          y = t->y, 
          z = t->z;

    t->x = cos * x - sin * z;
    t->y = y;    
    t->z = sin * x + cos * z;
}

void rotatez_i(Point* t, float cos, float sin) {
    float x = t->x, 
          y = t->y, 
          z = t->z;

    t->x =  cos * x + sin * y;
    t->y = -sin * x + cos * y;
    t->z = z;
}

/* Apply (inverse!) transformation of rotation in a model from pc, as slide 27 
 * from class 4
 *     pc: {-x/2, -y/2, -z/2}
 *     qc: {D/2, D/2, -D/2} 
 * */
void transform_i(Point* s, Point pc, Point qc, float thetax_cos, 
                 float thetax_sin, float thetay_cos, float thetay_sin) {
    translate(s, qc, true);
    rotatey_i(s, thetay_cos, thetay_sin);
    rotatex_i(s, thetax_cos, thetax_sin);
    translate(s, pc, true);
}

/* Rotate a model given a view (typically, it would be user's mouse) */
GrayImage* planar_cut(MedicalImage model, Point p1, Vector view) {
    int i, j;
    bool rotateyz;

    int x = model.nx,
        y = model.ny,
        z = model.nz;

    /* Create image */
    Point size = {x, y, z};
    float D = diagonal(size);
    GrayImage* output = CreateGrayImage(D + 1, D + 1);

    /* ((User assign p1 (for now))) */
    // Point p1 = {(float)x/2, (float)y/2, (float)z/2};
    Point qc = {-(float)D/2, -(float)D/2, (float)D/2};

    float Vx, Vy, Vz, Vz_, alphax, alphay, alphaz;

    /* Inicialize values (a.k.a. sanity check) */  
    Vx  = view.x;
    Vy  = view.y;
    Vz  = view.z;
    Vz_ = 0;
    alphax = 0;
    alphay = 0;
    alphaz = 0;

    /* Set things up */
    rotateyz = false;
    normalize(&view);

    /* Find out alphax, alphay and alphaz, according to slide no. 22 from
     * class 4.
     * */
    if (Vz == 0) {
        if (Vx == 0 && Vy != 0) {
            alphay = 0;
            alphax = 90 * sign(Vy);
        } else if (Vx != 0 && Vy == 0) {
            alphay = 90 * sign(Vx);
            alphax = 0;
        } else if (Vx != 0 && Vy != 0) {
            alphay = -90;
            alphaz = -sign(Vy) * acosf(Vx);

            rotateyz = true;
        }
    } else {
        alphax = atanf(view.y/view.z);
        Vz_    = (view.z)/cosf(alphax);

        alphay = atanf(view.x/Vz_);

        if (Vz < 0) {
            alphax = 180 - alphax;
            alphay = 180 - alphay;
        }
    }

    for (i = 0; i < D; i++) {
        for (j = 0; j < D; j++) {
            Point q = {j, i, -D/2};

            /* Apply phi, since p would be phi_i(q) */
            translate(&q, qc, false);

            if (rotateyz) {
                rotatey_i(&q, cosf(alphay), sinf(alphay));
                rotatez_i(&q, cosf(alphaz), sinf(alphaz));
            } else {
                rotatey_i(&q, cosf(-alphay), sinf(alphay));
                rotatex_i(&q, cosf(alphax), sinf(alphax));
            }

            translate(&q, p1, false);

            /* Finally, finds out I(p) from model */
            if (q.x < x && q.y < y && q.z < z &&
                q.x > 0 && q.y > 0 && q.z > 0) {
                output->val[i][j] = ImageValueAtPoint(&model, q);
            }
        }
    }

    return output;
}

/* Apply maximum intensity projection at model */
GrayImage* mip(MedicalImage model, Vector view) {
    int i, j, k;

    int x = model.nx,
        y = model.ny,
        z = model.nz;

    /* Create image */
    Point size = {x, y, z};
    float D = diagonal(size);
    GrayImage* output = CreateGrayImage(D + 1, D + 1);

    /* Find respective angles based on view vector */
    float thetax_cos, thetax_sin, thetay_cos, thetay_sin;
    find_ang(view, &thetax_cos, &thetax_sin, &thetay_cos, &thetay_sin);

    Vector n = {0, 0, 1};

    Point pc = {-(float)x/2, -(float)y/2, -(float)z/2};
    Point qc = {D/2, D/2, -D/2};

    /* Collect point of center from each face */
    Point cn[N_FACES];
    for (i = 0; i < N_FACES; i++) {
        cn[i] = product_1b1(c_face[i], size);
    }

    for (i = 0; i < D; i++) {
        for (j = 0; j < D; j++) {
            /* Point in plane + placeholders */
            Point q  = {j, i, -D/2},
                  n_ = n;

            transform_i(&q, pc, qc, thetax_cos, thetax_sin, thetay_cos, 
                        thetay_sin);
            transform_i(&n_, pc, qc, thetax_cos, thetax_sin, thetay_cos, 
                        thetay_sin);

            normalize(&n_);

            // printf("q: {%f, %f, %f}\n", q.x, q.y, q.z);

            /* Find p1 and pn */
            Point p1 = origin, 
                  pn = origin;
            float max = 0, min = D;

            /* Which faces does it belong to? Find out p1 and pn! */
            for (k = 0; k < N_FACES; k++) {
                Point p;
                float lambda;

                /* Find out lambda */
                lambda = (dot_product(face[k], cn[k]) - 
                          dot_product(face[k], q))/dot_product(face[k], n_);

                p = add_points(q, scalar_mul(lambda, n_));

                /* Check biggest and smallest value of lambda */
                if (within_limits(p, size) && lambda > 0) {
                    // printf("p: {%f, %f, %f}\n", p.x, p.y, p.z);

                    if (lambda > max) {
                        max = lambda;
                        pn  = p;
                    } else if (lambda < min) {
                        min = lambda;
                        p1  = p;
                    }
                }
            }

            if (max == 0 && min == D) {
                printf("%d, %d\n", i, j);
            }

            // printf("max: {%f, %f, %f} l: %f, min: {%f, %f, %f} l:%f\n", pn.x, pn.y, pn.z,
            //         max, p1.x, p1.y, p1.z, min);
        }
    }

    return output;
}

/* Projecoes de medias intensidades:
 *   Aplica rotacao inversa para aplicar em cada ponto do plano:
 *     pi =    T(D/2, D/2, D/2) Ry(Oy) Rx(Ox) T(-nx/2, -ny/2, -nz/2)
 *     pi^-1 = T(nx/2, ny/2, nz/2) Rx_i(Ox) Ry_i(Oy) T(-D/2, -D/2, -D/2)
 *   pi^-1(q') = pi^-1(q) + lambda*pi^-1(n)
 *   n := {0, 0, 1  }
 *   acha a face mais proxima para realizar a operacao
 *   anda com o algoritmo do dda
 *   ve quem eh o maximo I(p) e projeta

 * achar lambda:
 *  faz um por face (6x).
 *  se tiver dois lambdas no dominio da cena: um é p1 (menor deles) e o outro pn (maior deles)
 *   2 ou maior: escolhe maior e menor _still_
 * */