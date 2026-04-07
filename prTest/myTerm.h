//
// Created by kchipson on 18.05.2020.
//

#ifndef MYTERM_H
#define MYTERM_H

#include <sys/ioctl.h>

enum colors {
    RED = 196,
    PEACH = 203,
    GREEN = 10,
    SOFT_GREEN = 192,
    BLUE = 20,
    BLACK = 16,
    GRAY = 240,
    WHITE = 15,
    DEFAULT = 0
};

int mt_clrScreen(void);
int mt_gotoXY(unsigned int col, unsigned int row);
int mt_getScreenSize(unsigned int *rows, unsigned int *cols);
int mt_setFGcolor(enum colors color);
int mt_setBGcolor(enum colors color);
int mt_setDefaultColorSettings(void);

#endif // MYTERM_H