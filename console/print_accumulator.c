#include "console.h"

int print_accumulator(int accumulator) {    
    mt_gotoXY (75, 2); 

    if (((accumulator >> 16) & 1) == 1) printf("+");
    printf("%04X", accumulator);

    mt_gotoXY (0, 31);
    return 0;
}