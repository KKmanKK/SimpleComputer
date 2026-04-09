#include "myTerm.h"
#include <stdio.h>

int mt_setdefaultcolor(void) {
    printf("\033[0m");
    return 0;
}
