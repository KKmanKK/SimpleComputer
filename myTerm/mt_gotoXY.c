#include "myTerm.h"

int mt_gotoXY(int x, int y) {
    if (x < 0 || y < 0)
        return -1;
    printf("\033[%d;%dH", y, x);
    return 0;
}