#include "rendering.h"

void usage () {
    Error(" Forma de uso incorreta. \n \
./to2d <input.scn> <label.scn> <output.ppm> <Vx> <Vy> <Vz> <Modo>\n \
    - Vetor: Vetor de visualizacao, {Vx, Vy, Vz}; \
    - Modo: Se deseja visualizar opacidade (OPACO) ou não (NORMAL).\n", 
    "mip.c");
}

int main (int argc, char *argv[]) {
    MedicalImage* model,
                * label;
    GrayImage*   output;

    Vector view;
    Mode mode = NORMAL;

    /* Make sure arguments are fine */
    if (argc != 8) {
        usage();
    }

    model = ReadMedicalImage(argv[1]);
    label = ReadMedicalImage(argv[2]);

    view.x = atof(argv[4]);
    view.y = atof(argv[5]); 
    view.z = atof(argv[6]);

    if (strcasecmp("NORMAL", argv[7]) == 0) {
        mode = NORMAL;
    } else if (strcasecmp("OPACITY", argv[7]) == 0) {
        mode = OPACITY;
    } else {
        usage();
    }

    /* Binarize label, since we are dealing with skull model */
    if (mode == NORMAL) {
        binarize(label, 60);
    }

    /* Begin rendering */
    output = rendering(model, label, view, mode);
    scale_colorspace(output, NONE, H);

    /* Write output */
    WriteGrayImage(output, argv[3]);

    /* Clean up the mess! */
    DestroyMedicalImage(&model);
    DestroyMedicalImage(&label);
    DestroyGrayImage(&output);

    return 0;
}