#ifndef __GUARD_MYTERM_H
#define __GUARD_MYTERM_H

#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

enum color {
    C_BLACK = 0,
    C_RED = 1,
    C_GREEN = 2,
    C_YELLOW = 3,
    C_BLUE = 4,
    C_MAGENTA = 5,
    C_CYAN = 6,
    C_WHITE = 7,
    C_DEFAULT = 9
};

int mt_clrscr(void);

int mt_gotoXY(int x, int y);

int mt_getscreensize(int* rows, int* cols);

int mt_setfgcolor(enum color color);

int mt_setbgcolor(enum color color);

#endif /* __GUARD_MYTERM_H */