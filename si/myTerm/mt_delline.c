#include "myTerm.h"
#include <stdio.h>

int mt_delline(void) {
    /* \033[M - delete current line (dl1) */
    printf("\033[M");
    return 0;
}
