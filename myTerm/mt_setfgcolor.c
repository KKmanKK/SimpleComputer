#include "myTerm.h"

int mt_setfgcolor(enum color color) {
    if (color < C_BLACK || color > C_DEFAULT) return -1;
    printf("\033[3%dm", color);
    return 0;
}