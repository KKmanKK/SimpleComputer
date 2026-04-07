#include "myTerm.h"

int mt_clrscr(void) {
    printf("\E[H\E[2J");
    printf("\033[H");
    return 0;
}