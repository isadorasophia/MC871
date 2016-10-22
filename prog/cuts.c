#include "visual.h"

void usage () {
    Error(" Forma de uso incorreta. \n \
./to2d <input.scn> <output.ppm> <Vx> <Vy> <Vz> <Px> <Py> <Pz>\n \
    - Vetor: Direcao do corte planar, {Vx, Vy, Vz};\n \
    - Point: Ponto de referencia para realizar o corte, {Px, Py, Pz}.\n", 
    "projection.c");
}

int main (int argc, char *argv[]) {
    MedicalImage* model;
    GrayImage*   output;

    Vector view;
    Point p;

    /* Make sure arguments are fine */
    if (argc != 9) {
        usage();
    }

    model = ReadMedicalImage(argv[1]);

    view.x = atof(argv[3]);
    view.y = atof(argv[4]); 
    view.z = atof(argv[5]);
    p.x    = atof(argv[6]);
    p.y    = atof(argv[7]);
    p.z    = atof(argv[8]);

    output = mip(*model, view);
    // output = planar_cut(*model, view, p);

    /* Write output */
    WriteGrayImage(output, argv[2]);

    /* Clean up the mess! */
    DestroyMedicalImage(&model);
    DestroyGrayImage(&output);

    return 0;
}