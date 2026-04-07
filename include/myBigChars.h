#ifndef __GUARD_MYBIGCHARS_H
#define __GUARD_MYBIGCHARS_H

#include <string.h>
#include <stdio.h>
#include <unistd.h>

#define BIG_SYMBOL_0 1111638588, 1010975298
#define BIG_SYMBOL_1 303306768, 2081427472
#define BIG_SYMBOL_2 541213244, 2114193424
#define BIG_SYMBOL_3 943866428, 1010975296
#define BIG_SYMBOL_4 606613536, 538976382
#define BIG_SYMBOL_5 1040319102, 1010974784
#define BIG_SYMBOL_6 1040335420, 1010975298
#define BIG_SYMBOL_7 270549118, 134744072
#define BIG_SYMBOL_8 1010975292, 1010975298
#define BIG_SYMBOL_9 1111638588, 1010974844
#define BIG_SYMBOL_PLUS 404226048, 1579134

#define BIG_SYMBOL_A 2118263832, 1111638594
#define BIG_SYMBOL_B 505553438, 2118271522
#define BIG_SYMBOL_C 33702460,1010975234
#define BIG_SYMBOL_D 1111638590, 1044529730
#define BIG_SYMBOL_E 503448126, 2114060802
#define BIG_SYMBOL_F 503448126, 33686018

enum char_color {
    BLACK = 0,
    RED = 1,
    GREEN = 2,
    YELLOW = 3,
    BLUE = 4,
    MAGENTA = 5,
    CYAN = 6,
    WHITE = 7,
    DEFAULT = 9
};

int bc_printA (char * str);

int bc_box(int x1, int y1, int x2, int y2);

int bc_printbigchar (int symbol[2], int x, int y, enum char_color fg_color, enum char_color bg_color);

int bc_setbigcharpos (int * big, int x, int y, int value);

int bc_getbigcharpos(int * big, int x, int y, int *value);

int bc_bigcharwrite(int fd, int *big, int count);

int bc_bigcharread(int fd, int *big, int need_count, int *count);

#endif /* __GUARD_MYBIGCHARS_H */