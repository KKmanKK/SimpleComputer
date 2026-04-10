#include "console.h"

void printFlags(void) {
    static const char letters[FLAG_COUNT] = {'M', 'E', '0', 'P', 'T'};
    int value;

    mt_gotoXY(FLAGS_ROW, RP_COL);

    for (int i = 1; i <= FLAG_COUNT; i++) {
        sc_regGet(i, &value);
        printf("%c", value ? letters[i - 1] : '_');
        if (i < FLAG_COUNT)
            printf(" ");
    }
}
