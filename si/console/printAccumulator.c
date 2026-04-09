#include "console.h"

void printAccumulator(void) {
    int value;
    sc_accumulatorGet(&value);

    mt_gotoXY(ACC_ROW,     RP_COL); printf("SC:  %-10d",  value);
    mt_gotoXY(ACC_ROW + 1, RP_COL); printf("HEX: %-10X",  (unsigned int)value);
}
