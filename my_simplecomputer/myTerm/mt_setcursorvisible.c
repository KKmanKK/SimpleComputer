#include "myTerm.h"
#include <stdio.h>

int mt_setcursorvisible(int value) {
    /* \033[?25h - show cursor */
    /* \033[?25l - hide cursor */
    printf(value ? "\033[?25h" : "\033[?25l");
    return 0;
}
