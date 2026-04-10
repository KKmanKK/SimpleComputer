#include "myTerm.h"
#include <stdio.h>

int mt_gotoXY(int row, int col) {
    if (row < 0 || col < 0)
        return -1;
    printf("\033[%d;%dH", row, col);
    return 0;
}
