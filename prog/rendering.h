#pragma once

#include "visual.h"

#define N_NEIGH 26
#define K       5

typedef enum { NORMAL = 1, OPACITY } Mode;

/* Control values */
double alpha[K]  = {0},
       acc_value = .4;

Point div_1b1(Point a, Point b) {
    Point res = {a.x/b.x, a.y/b.y, a.z/b.z};

    return res;
}

Point scalar_div(double a, Point b) {
    Point res = {b.x/a, b.y/a, b.z/a};

    return res;
}

void binarize(MedicalImage* img, int threshold) {
    int x, y, z;

    for (x = 0; x < img->nx; ++x) {
        for (y = 0; y < img->ny; ++y) {
            for (z = 0; z < img->nz; ++z) {
                if (img->val[z][y][x] > threshold) {
                    img->val[z][y][x] = 1;
                } else {
                    img->val[z][y][x] = 0;
                }
            }
        }
    }
}

double get_value(MedicalImage* mdl, MedicalImage* lbl, Point t, int m) {
    Point size = {mdl->nx, mdl->ny, mdl->nz};
    double res;

    if (within_limits(t, size)) {
        res = ImageValueAtPoint(mdl, t);

        if (ImageValueAtPoint(lbl, t) != m) {
            res = 0;
        }

    } else {
        res = 0;
    }

    return res;
}

/* Find point with a given label
 *   return new label, if found
 *   else just return old label
 * */
int find_point(MedicalImage* lbl, Point* target, int old_label) {
    int i, v = old_label;

    AdjRel* neigh = Spheric(1);
    Point size = {lbl->nx, lbl->ny, lbl->nz};

    for (i = 0; i < neigh->n; ++i) {
        Point t = {neigh->adj[i].dx, neigh->adj[i].dy, neigh->adj[i].dz};
        
        /* t := t + target */
        t = add_points(*target, t);

        if (within_limits(t, size)) {
            v = lbl->val[(int)t.z][(int)t.y][(int)t.x];
        } else {
            continue;
        }

        if (v != old_label) {
            target->x = t.x;
            target->y = t.y;
            target->z = t.z;

            return v;
        }
    }

    return v;
}

Point derivate(MedicalImage* mdl, Point target, Point neigh,
               MedicalImage* lbl, double mask) {
    Point t, res = {0, 0, 0}; 
    double delta_norm;
    int value_p; // value_m;

    t = add_points(target, neigh);
    value_p = get_value(mdl, lbl, t, mask);

    // unnecessary
    // t = sub_points(target, neigh);
    // value_m = get_value(mdl, lbl, t, mask);

    delta_norm = 2 * diagonal(neigh);

    /* Reset */
    if (delta_norm == 0) {
        return res;
    }

    res = scalar_div(delta_norm, scalar_mul(value_p, neigh));

    return res;
}

/* adjacency.h e percorre toda a lista (só considera com o mesmo label) */
Point find_normal(MedicalImage* mdl, Point target, MedicalImage* lbl,
                  double mask) {
    int i;

    Point normal  = {0, 0, 0};
    AdjRel* neigh = Spheric(6);

    /* Start applying for all neighbors */
    for (i = 0; i < neigh->n; ++i) {
        Point t = {neigh->adj[i].dx, neigh->adj[i].dy, neigh->adj[i].dz};
        t = derivate(mdl, target, t, lbl, mask);

        normal = add_points(normal, t);
    }

    /* Invert values */
    scalar_mul(-1, normal);
    normalize(&normal);

    /* Clean up the mess... */
    DestroyAdjRel(&neigh);

    return normal;
}

/* p: é o ponto do plano e q é o ponto no modelo */
float dist(Point p, Point q, double D) {
    double diag = diagonal(sub_points(p, q));

    return H*(1 - .8 * diag/(D) + .2);
}

/* cos theta = Normal(p) * (phi^-1(n)) => theta = 1/(cos theta) */
double find_theta(Point normal, Point n_) {
    double res = acos(dot_product(normal, n_));

    return res;
}

/* L(p) operation in Phong model */
int L(MedicalImage* mdl, MedicalImage* lbl, Point p,
      Point q, Point n_, double D, double mask) {
    /* Constants for Phong operation */
    double ka = 0.2,
           kd = 0.5,
           ks = 0.3,
           ns = 5,
           La = H;

    double theta, dist_p, res = 0;

    /* Estimate normal */
    Point normal = find_normal(mdl, p, lbl, mask);

    /* Find best approximation to normal, according to view angle */
    theta = find_theta(normal, n_);

    /* Get D(p) */
    dist_p = dist(p, q, D);

    /* Is it a valid theta for our calculations? */
    if (0 <= theta && theta <= M_PI/2) {
        res = dist_p * kd * cos(theta) + ka*La;

        /* Should we also add the spectrum? */
        if (theta <= M_PI/4) {
            res += dist_p * ks * pow(cos(2*theta), ns);
        }

    }

    return res;
}

/* Implementation of DDA algorithm in 3D, returns maximum 
 * intensity within p1 to pn with Phong function with opacity!
 *   OBS: parameters were fit to brain scene
 * */
int DDA3d_phong_opacity(MedicalImage* mdl, MedicalImage* lbl, 
                   Point p1, Point pn, double D, Point q, Point n_) {
    int k, n, i, t = 0;
    double Dx = 0, Dy = 0, Dz = 0, 
           dx = 0, dy = 0, dz = 0;

    double res = 0;

    int ref   = ImageValueAtPoint(lbl, p1),
        ref_c = ref;

    bool first      = true,
         visited[K] = {false};

    find_interval(&p1, &pn, &Dx, &Dy, &Dz, &dx, &dy, &dz, &n);

    Point p   = p1,
          p_t = {0, 0, 0};

    for (k = 0; k < n; k++) {
        p_t = p;
        t = find_point(lbl, &p_t, ref_c);

        /* Makes sure it is a new label, different from background and
         * wasn't visited yet */
        if (t != ref_c && t != ref && visited[t % K] == false) {
            /* Check if it is unitialized */
            if (alpha[t % K] == 0) {
                alpha[t % K] = acc_value;
            }

            /* If it isn't the first time we got a different label value */
            if (!first) {
                double v = alpha[t % K]*L(mdl, lbl, p_t, q, n_, D, t);

                for (i = 0; i < K; ++i) {
                    if (i != t % K && visited[i] == true) {
                        v = v * (1 - alpha[i]);
                    }
                }

                res += v;

            } else {
                res = alpha[t % K]*L(mdl, lbl, p_t, q, n_, D, t);

                /* Ok, we were in the first time */
                first = false;
            }

            visited[t % K] = true;
        }

        ref_c = t;

        p.x += dx;
        p.y += dy;
        p.z += dz;
    }

    return res;
}

/* Implementation of DDA algorithm in 3D, returns maximum 
 * intensity within p1 to pn with Phong function
 * */
int DDA3d_phong(MedicalImage* mdl, MedicalImage* lbl, 
                   Point p1, Point pn, double D, Point q, Point n_) {
    int k, n, t;
    double Dx = 0, Dy = 0, Dz = 0, 
           dx = 0, dy = 0, dz = 0;

    int ref = ImageValueAtPoint(lbl, p1);

    find_interval(&p1, &pn, &Dx, &Dy, &Dz, &dx, &dy, &dz, &n);

    Point p = p1,
          p_t;

    for (k = 0; k < n; k++) {
        p_t = p;
        t = find_point(lbl, &p_t, ref);

        if (t > ref) {
            return L(mdl, lbl, p_t, q, n_, D, t);
        }

        p.x += dx;
        p.y += dy;
        p.z += dz;
    }

    return 0;
}

GrayImage* rendering(MedicalImage* mdl, MedicalImage* lbl, Vector view,
                     Mode mode) {
    int i, j, k;

    int x = mdl->nx,
        y = mdl->ny,
        z = mdl->nz;

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
                  q_  = q,
                  n_  = n;
            bool found = false;

            /* Apply inverse transformation on q and n_ */
            transform_i(&q_, pc, qc, thetax_cos, thetax_sin, thetay_cos, 
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
                lambda = dot_product(face[k], sub_points(cn[k], q_)) / 
                         (dot_product(face[k], n_) + THRESH);

                p = add_points(q_, scalar_mul(lambda, n_));

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
                if (mode == NORMAL) {
                    output->val[i][j] = DDA3d_phong(mdl, lbl, p1, pn, D, 
                                                    q, n_);
                } else {
                    output->val[i][j] = DDA3d_phong_opacity(mdl, lbl, p1, pn, D, 
                                                            q, n_);
                }
            }
        }
    }

    return output;
}
