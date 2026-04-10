#ifndef MYTERM_H
#define MYTERM_H

#include <unistd.h>

#ifndef STDOUT_FILENO
#define STDOUT_FILENO 1
#endif

enum colors {
    C_BLACK   = 0,
    C_RED     = 1,
    C_GREEN   = 2,
    C_YELLOW  = 3,
    C_BLUE    = 4,
    C_MAGENTA = 5,
    C_CYAN    = 6,
    C_WHITE   = 7,
    C_DEFAULT = 9
};

int mt_clrscr(void);
int mt_gotoXY(int row, int col);
int mt_getscreensize(int *rows, int *cols);
int mt_setfgcolor(enum colors color);
int mt_setbgcolor(enum colors color);
int mt_setdefaultcolor(void);
int mt_setcursorvisible(int value);
int mt_delline(void);

#endif /* MYTERM_H */