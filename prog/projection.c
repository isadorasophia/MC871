#include "wireframe.h"

void usage () {
    Error(" Forma de uso incorreta. \n \
./to2d <input.scn> <output.ppm> <x> <y> <z>\n \
    - Vetor: Ponto de vista do usuario, traduzido em x y z.\n", "projection.c");
}

int main (int argc, char *argv[]) {
    MedicalImage* model;
    ColorImage*   output;

    Vector view;

    /* Make sure arguments are fine */
    if (argc != 6) {
        usage();
    }

    model = ReadMedicalImage(argv[1]);

    view.x = atof(argv[3]);
    view.y = atof(argv[4]);
    view.z = atof(argv[5]);

    output = draw_wireframe(*model, view);

    /* Write output */
    WriteColorImage(output, argv[2]);

    /* Clean up the mess! */
    DestroyMedicalImage(&model);
    DestroyColorImage(&output);

    return 0;
}