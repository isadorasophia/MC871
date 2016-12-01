#pragma once

#include <float.h>

#include "basic.h"
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

Point scalar_mul(double l, Point a) {
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

void rotatex_i(Point* t, double cos, double sin) {
    double x = t->x, 
          y = t->y, 
          z = t->z;

    t->x = x;
    t->y =  cos * y + sin * z;
    t->z = -sin * y + cos * z;
}

void rotatey_i(Point* t, double cos, double sin) {
    double x = t->x, 
          y = t->y, 
          z = t->z;

    t->x = cos * x - sin * z;
    t->y = y;
    t->z = sin * x + cos * z;
}

void rotatez_i(Point* t, double cos, double sin) {
    double x = t->x, 
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
void transform_i(Point* s, Point pc, Point qc, double thetax_cos, 
                 double thetax_sin, double thetay_cos, double thetay_sin) {
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
    double D = diagonal(size);
    GrayImage* output = CreateGrayImage(D + 1, D + 1);

    Point qc = {(double)D/2, (double)D/2, -(double)D/2};

    double Vx, Vy, Vz, Vz_, alphax, alphay, alphaz;

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
            alphaz = -sign(Vy) * acos(Vx);

            rotateyz = true;
        }
    } else {
        alphax = atanf(view.y/(view.z + THRESH));
        Vz_    = (view.z)/(cos(alphax) + THRESH);

        alphay = atanf(view.x/(Vz_ + THRESH));

        if (Vz < 0) {
            alphax = 180 - alphax;
            alphay = 180 - alphay;
        }
    }

    for (i = 0; i < D; i++) {
        for (j = 0; j < D; j++) {
            Point q = {j, i, -D/2};

            /* Apply phi, since p would be phi_i(q) */
            translate(&q, qc, true);

            if (rotateyz) {
                rotatey_i(&q, cos(alphay), sin(alphay));
                rotatez_i(&q, cos(alphaz), sin(alphaz));
            } else {
                rotatey_i(&q, cos(-alphay), sin(alphay));
                rotatex_i(&q, cos(alphax), sin(alphax));
            }

            translate(&q, p1, false);

            /* Finally, finds out I(p) from model */
            if (q.x < x && q.y < y && q.z < z &&
                q.x > 0 && q.y > 0 && q.z > 0) {
                Point p;

                p.x = floor(p.x);
                p.y = floor(p.y);
                p.z = floor(p.z);

                output->val[i][j] = ImageValueAtPoint(&model, q);
            }
        }   
    }

    return output;
}

void find_interval(Point* p1, Point* pn,
                   double* Dx, double* Dy, double* Dz,
                   double* dx, double* dy, double* dz,
                   int* n) {
    p1->x = floor(p1->x);
    p1->y = floor(p1->y);
    p1->z = floor(p1->z);

    pn->x = floor(pn->x);
    pn->y = floor(pn->y);
    pn->z = floor(pn->z);

    if (p1->x == pn->x && p1->y == pn->y && p1->z == pn->z) {
        *n = 1;
    } else {
        *Dx = pn->x - p1->x;
        *Dy = pn->y - p1->y;
        *Dz = pn->z - p1->z;

        if (abs(*Dx) >= abs(*Dy) && abs(*Dx) >= abs(*Dz)) {
            *n = abs(*Dx) + 1;
            (*dx) = sign(*Dx);
            (*dy) = (*dx)*(*Dy)/(*Dx);
            (*dz) = (*dx)*(*Dz)/(*Dx);

        } else if (abs(*Dy) >= abs(*Dx) && abs(*Dy) >= abs(*Dz)) {
            *n = abs(*Dy) + 1;
            (*dy) = sign(*Dy);
            (*dx) = (*dy)*(*Dx)/(*Dy);
            (*dz) = (*dy)*(*Dz)/(*Dy);

        } else {
            *n = abs(*Dz) + 1;
            (*dz) = sign(*Dz);
            (*dx) = (*dz)*(*Dx)/(*Dz);
            (*dy) = (*dz)*(*Dy)/(*Dz);

        }
    }
}

/* Implementation of DDA algorithm in 3D, returns maximum 
 * intensity within p1 to pn 
 * */
double DDA3d_avg(MedicalImage model, Point p1, Point pn) {
    int k, n, counter;
    double Dx = 0, Dy = 0, Dz = 0, 
           dx = 0, dy = 0, dz = 0;

    double t, avg;

    find_interval(&p1, &pn, &Dx, &Dy, &Dz, &dx, &dy, &dz, &n);

    Point p = p1;
    avg = 0;
    counter = 0;

    for (k = 0; k < n; k++) {
        t = ImageValueAtPoint(&model, p);

        ++counter;
        avg += t;

        p.x += dx;
        p.y += dy;
        p.z += dz;
    }

    avg /= counter;

    return avg;
}

/* Implementation of DDA algorithm in 3D, returns maximum 
 * intensity within p1 to pn 
 * */
double DDA3d_max(MedicalImage model, Point p1, Point pn) {
    int k, n;
    double Dx = 0, Dy = 0, Dz = 0, 
           dx = 0, dy = 0, dz = 0;

    double t, max = 0;

    find_interval(&p1, &pn, &Dx, &Dy, &Dz, &dx, &dy, &dz, &n);

    Point p = p1;

    for (k = 0; k < n; k++) {
        t = ImageValueAtPoint(&model, p);

        if (t > max) {
            max = t;
        }

        p.x += dx;
        p.y += dy;
        p.z += dz;
    }

    return max;
}

/* Apply maximum intensity projection at model */
GrayImage* mip(MedicalImage model, Vector view) {
    int i, j, k;

    int x = model.nx,
        y = model.ny,
        z = model.nz;

    /* Create image */
    Point size = {x, y, z};
    double D = diagonal(size);
    GrayImage* output = CreateGrayImage(D + 1, D + 1);

    /* Find respective angles based on view vector */
    double thetax_cos, thetax_sin, thetay_cos, thetay_sin;
    find_ang(view, &thetax_cos, &thetax_sin, &thetay_cos, &thetay_sin);

    Vector n = {0, 0, 1};

    Point pc = {-(double)x/2, -(double)y/2, -(double)z/2};
    Point qc = {D/2, D/2, D/2};

    /* Collect point of center from each face */
    Point cn[N_FACES];
    for (i = 0; i < N_FACES; i++) {
        cn[i] = product_1b1(c_face[i], size);
    }

    for (i = 0; i < D; i++) {
        for (j = 0; j < D; j++) {
            /* Point in plane + placeholders */
            Point q   = {j, i, -D/2},
                  n_  = n;
            bool found = false;

            /* Apply inverse transformation on q and n_ */
            transform_i(&q, pc, qc, thetax_cos, thetax_sin, thetay_cos, 
                        thetay_sin);
            transform_i(&n_, origin, origin, thetax_cos, thetax_sin, thetay_cos, 
                        thetay_sin);

            /* Initialize */
            Point p1 = origin, 
                  pn = origin;
            double max = 0, min = DBL_MAX;

            /* Which faces does it belong to? Find out p1 and pn! */
            for (k = 0; k < N_FACES; k++) {
                Point p;
                double lambda;

                /* Find out lambda */
                lambda = dot_product(face[k], sub_points(cn[k], q)) / 
                         (dot_product(face[k], n_) + THRESH);

                p = add_points(q, scalar_mul(lambda, n_));

                /* Check biggest and smallest value of lambda */
                if (within_limits(p, size)) {
                    found = true;

                    if (lambda > max) {
                        max = lambda;
                        pn  = p;
                    } 

                    if (lambda < min) {  
                        min = lambda;
                        p1  = p;
                    }
                } 
            }

            if (found && min < max) {
                output->val[i][j] = DDA3d_max(model, p1, pn);
            }
        }
    }

    return output;
}
