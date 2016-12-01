#include "reformat.h"

void usage () {
    Error(" Forma de uso incorreta. \n \
./to2d <input.scn> <output.ppm> <P1x> <P1y> <P1z> <Pnx> <Pny> <Pnz> <n>\n \
    - P1: Ponto de inicio;\n \
    - Pn: Ponto de fim de extracao do corte;\n \
    - n:  Numero de cortes a serem realizados.\n", 
    "cuts.c");
}

int main (int argc, char *argv[]) {
    MedicalImage* model;
    GrayImage*   output;

    Point p1, pn;
    int n;

    /* Make sure arguments are fine */
    if (argc != 10) {
        usage();
    }

    model = ReadMedicalImage(argv[1]);

    p1.x = atof(argv[3]);
    p1.y = atof(argv[4]); 
    p1.z = atof(argv[5]);
    pn.x = atof(argv[6]);
    pn.y = atof(argv[7]);
    pn.z = atof(argv[8]);
    n = atoi(argv[9]);

    output = reformat(model, p1, pn, n);

    /* Write output */
    WriteGrayImage(output, argv[2]);

    /* Clean up the mess! */
    DestroyMedicalImage(&model);
    DestroyGrayImage(&output);

    return 0;
}