#include "console.h"

static void print_binary(int value) {
    if (value == 0) {
        printf("0");
        return;
    }
    int bits = sizeof(int) * 8;
    int started = 0;
    for (int i = bits - 1; i >= 0; i--) {
        int bit = (value >> i) & 1;
        if (bit) started = 1;
        if (started) printf("%d", bit);
    }
}

void printDecodedCommand(int value) {
    mt_gotoXY(DECODED_ROW,     RP_COL); printf("DEC: %-10d",  value);
    mt_gotoXY(DECODED_ROW + 1, RP_COL); printf("OCT: %-10o",  value);
    mt_gotoXY(DECODED_ROW + 2, RP_COL); printf("HEX: %-10X",  value);
    mt_gotoXY(DECODED_ROW + 3, RP_COL); printf("BIN: ");
    print_binary(value);
    printf("          ");  /* overwrite stale digits */
}
