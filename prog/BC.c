#include "ext_image.h"

void usage () {
    Error("Forma de uso incorreta. \n \
./to2d <input.scn> <labeled.scn> <output.ppm> \n \
       <tipo de corte> <origem de corte> <brilho> <contraste>\n \
    - Tipo de corte: AXIAL, CORONAL ou SAGITAL;\n \
    - Origem de corte: coordenada de origem do corte (ou OP para opcional);\n \
    - B e C: valor de 0 a 100.\n", "to2d.c");
}

int main (int argc, char *argv[]) {
    MedicalImage* model;
    MedicalImage* label;
    GrayImage*    img;
    ColorImage*        color_img;

    int x, y, cord;
    int B, C;

    Cut cut;

    /* Make sure arguments are fine */
    if (argc != 8) {
        usage();
    }

    model = ReadMedicalImage(argv[1]);
    label = ReadMedicalImage(argv[2]);

    /* Make sure both images fit each other */
    if (model->nx != label->nx || model->ny != label->ny ||
        model->nz != label->nz) {
        printf("Incorrect input images, please double check them!\n");
        usage();
    }

    if (strcasecmp("OP", argv[5]) == 0 ||
        strcasecmp("Valor da origem...", argv[5]) == 0) {
        cord = NONE;
    } else {
        cord = atoi(argv[5]);
    }

    /* Brightness and contrast */
    B = atoi(argv[6]);
    C = atoi(argv[7]);

    /* Get cut type */
    if (strcasecmp("AXIAL", argv[4]) == 0) {
        y = model->nx;
        x = model->ny;
        cut = AXIAL;

    } else if (strcasecmp("CORONAL", argv[4]) == 0) {
        y = model->nx;
        x = model->nz;
        cut = CORONAL;

    } else if (strcasecmp("SAGITAL", argv[4]) == 0) {
        y = model->ny;
        x = model->nz;
        cut = SAGITAL;

    } else {
        printf("Tipo de corte incorreto!\n");

        usage();
    }

    img = CreateGrayImage(y, x);
    color_img = CreateColorImage(y, x);

    /* get label model */
    to2d_cut(label, &img, cut, cord);

    if (color(img, color_img) == OK) {
        /* get cut from original model */
        to2d_cut(model, &img, cut, cord);

        /* convert colorspace from model to H */
        scale_colorspace(img, NONE, H);

        BC(img, B, C);

        /* apply mask! */
        apply_mask(img, color_img);

        /* Write output */
        WriteColorImage(color_img, argv[3]);
    } else {
        printf("Incorrect usage!\n");
    }

    /* Clean up the mess! */
    DestroyMedicalImage(&model);
    DestroyMedicalImage(&label);
    DestroyGrayImage(&img);
    DestroyColorImage(&color_img);

    return 0;
}