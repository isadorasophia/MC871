#include "basic.h"

/* Aplicacao que testa a funcionalidade da biblioteca to2d.h,
 * responsavel por converter imagens 3D em 2D */

void usage () {
    Error("Forma de uso incorreta. \n \
./to2d <input.scn> <output.pgm> <eixo de corte> \n \
       <orientacao> <coordenada de origem>\n \
    - Eixo de corte: X, Y ou Z;\n \
    - Orientacao: IN ou OUT;\n \
    - Coordenada de origem: regiao de corte.\n", "to2d.c");
}

int main (int argc, char *argv[]) {
    MedicalImage* original;
    GrayImage*    output;
    Axis          a = XY;
    Orientation   o = IN;

    int           x = 0, y = 0, origin;

    /* Make sure arguments are fine */
    if (argc != 6) {
        usage();
    }

    original = ReadMedicalImage(argv[1]);

    /* Check axis */    
    if (strcasecmp("X", argv[3]) == 0) {
        a = ZY;
    } else if (strcasecmp("Z", argv[3]) == 0) {
        a = XY;
    } else if (strcasecmp("Y", argv[3]) == 0) {
        a = XZ;
    } else {
        printf("Incorrect axis.\n");

        usage();
    }

    /* Get orientation */
    if (strcasecmp("IN", argv[4]) == 0) {
        o = IN;
    } else if (strcasecmp("OUT", argv[4]) == 0) {
        o = OUT;
    } else {
        printf("Incorrect orientation!\n");

        usage();
    }

    /* Get origin */
    origin = atoi(argv[5]);

    /* Now, set coordinates accordingly */
    if (a == XY) {
        /* Make sure the coordinate is ok */
        if (origin >= original->nz) {
            goto invalid_axis;
        }

        switch (o) {
            case IN: // XY
                x = original->nx;
                y = original->ny;

                break;

            case OUT: // YX
                x = original->ny;
                y = original->nx;

                break;
        }

    } else if (a == XZ) {
        /* Make sure the coordinate is ok */
        if (origin >= original->ny) {
            goto invalid_axis;
        }

        switch (o) {
            case IN: // XZ
                x = original->nx;
                y = original->nz;

                break;

            case OUT: // ZX
                x = original->nz;
                y = original->nx;

                break;
        }

    } else {
        /* Make sure the coordinate is ok */
        if (origin >= original->nx) {
            goto invalid_axis;
        }

        switch (o) {
            case IN: // ZY
                x = original->nz;
                y = original->ny;

                break;

            case OUT: // YZ
                x = original->ny;
                y = original->nz;

                break;

            default:
            invalid_axis:
            {
                printf("Coordenada de origem incorreta (ultrapassa limite)!\n");

                usage();
            }
        }
    }

    /* Create 2D image, as output */
    output = CreateGrayImage(x, y);

    /* Transcript voxels into the 2D image */
    to2d(original, &output, a, o, x, y, origin);

    /* Write output */
    WriteGrayImage(output, argv[2]);

    /* Clean up the mess! */
    DestroyMedicalImage(&original);
    DestroyGrayImage(&output);

    return 0;
}