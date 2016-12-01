#include "basic.h"

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
    ColorImage*   color_img;

    /* Flag to use label or not */
    int use_label = 1;

    int x = 0, 
        y = 0, 
        cord = 0;
    int B, C;

    Cut cut = AXIAL;

    /* Make sure arguments are fine */
    if (argc != 8) {
        usage();
    }

    if (strcasecmp("NONE", argv[2]) == 0) {
        use_label = 0;
    }

    model = ReadMedicalImage(argv[1]);

    if (use_label) {
        label = ReadMedicalImage(argv[2]);
    }

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

    if (use_label) {
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

        /* free memory */
        DestroyMedicalImage(&label);
    } else {
        /* simply apply brightness and constrast + cut */
        /* first, get model to plane */
        to2d_cut(model, &img, cut, cord);

        BC(img, B, C);

        /* Write output */
        WriteGrayImage(img, argv[3]);
    }

    /* Clean up the mess! */
    DestroyMedicalImage(&model);
    DestroyGrayImage(&img);
    DestroyColorImage(&color_img);

    return 0;
}