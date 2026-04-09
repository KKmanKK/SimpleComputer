#include "myTerm.h"
#include <stdio.h>

int mt_setbgcolor(enum colors color) {
    if (color < C_BLACK || color > C_DEFAULT)
        return -1;
    printf("\033[4%dm", color);
    return 0;
}
