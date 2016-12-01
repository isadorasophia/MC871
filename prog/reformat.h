#pragma once

#include "rendering.h"

void acc_images(GrayImage* a, GrayImage** b) {
    if (a->nx != (*b)->nx || a->ny != (*b)->ny) {
        DestroyGrayImage(b);

        return;
    }

    int x, y;

    for (y = 0; y < a->ny; ++y) {
        for (x = 0; x < a->nx; ++x) {
            int v = (*b)->val[y][x];

            if (v > H) {
                a->val[y][x] = v;
            }
        }
    }

    DestroyGrayImage(b);
}

GrayImage* reformat(MedicalImage* mdl, Point p1, Point pn, 
                    int n) {
    Point  sub = sub_points(pn, p1);
    normalize(&sub);

    Vector v   = sub;

    GrayImage* res, 
             * t;

    /* Create image */
    int x = mdl->nx,
        y = mdl->ny,
        z = mdl->nz,
        k,
        n_p;

    Point size = {x, y, z};
    double D = diagonal(size);
    res = CreateGrayImage(2*D + 1, 2*D + 1);

    double Dx = 0, Dy = 0, Dz = 0, 
           dx = 0, dy = 0, dz = 0;

    find_interval(&p1, &pn, &Dx, &Dy, &Dz, &dx, &dy, &dz, &n_p);

    Point p = p1;

    /* Initialize */
    res = planar_cut(*mdl, p, v);
    p.x += dx;
    p.y += dy;
    p.z += dz;

    for (k = 1; k < n_p; k++) {
        t = planar_cut(*mdl, p, v);
        acc_images(res, &t);

        p.x += dx;
        p.y += dy;
        p.z += dz;
    }

    return res;
}