#include "visual.h"

void usage () {
    Error(" Forma de uso incorreta. \n \
./to2d <input.scn> <output.ppm> <Vx> <Vy> <Vz>\n \
    - Vetor: Vetor de visualizacao, {Vx, Vy, Vz};\n", 
    "mip.c");
}

int main (int argc, char *argv[]) {
    MedicalImage* model;
    GrayImage*   output;

    Vector view;

    /* Make sure arguments are fine */
    if (argc != 6) {
        usage();
    }

    model = ReadMedicalImage(argv[1]);

    view.x = atof(argv[3]);
    view.y = atof(argv[4]); 
    view.z = atof(argv[5]);

    output = mip(*model, view);
    scale_colorspace(output, NONE, H);

    /* Write output */
    WriteGrayImage(output, argv[2]);

    /* Clean up the mess! */
    DestroyMedicalImage(&model);
    DestroyGrayImage(&output);

    return 0;
}