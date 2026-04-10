#include "myTerm.h"
#include <stdio.h>

int mt_clrscr(void) {
    /* \033[H  - move cursor to top-left */
    /* \033[2J - erase entire screen     */
    printf("\033[H\033[2J");
    return 0;
}
