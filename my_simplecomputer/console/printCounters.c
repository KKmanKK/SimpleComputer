#include "console.h"

void printCounters(void) {
    int value;
    sc_icounterGet(&value);

    mt_gotoXY(IC_ROW, RP_COL);
    printf("IC: %-4d", value);
}
