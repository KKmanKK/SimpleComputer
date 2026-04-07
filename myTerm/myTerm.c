#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "../include/myTerm.h"

int mt_clrscr(void) {
    printf("\E[H\E[2J");
    printf("\033[H");
    return 0;
}

int mt_gotoXY(int x, int y) {
    if (x < 0 || y < 0) return -1;
    printf("\033[%d;%dH", y, x);
    return 0;
}

int mt_getscreensize(int* rows, int* cols) {
    if (rows == NULL || cols == NULL) return -1;
    else {
        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        *rows = w.ws_row;
        *cols = w.ws_col;
        return 0;
    }
}

int mt_setfgcolor(enum color color) {
    if (color < 0 || color > 7) return -1;
    printf("\033[3%dm", color);
    return 0;
}

int mt_setbgcolor(enum color color) {
    if (color < 0 || color > 7) return -1;
    printf("\033[4%dm", color);
    return 0;
}

// int mt_setdefaultcolor(void) {
//     printf("\033[0m");
//     return 0;
// }

// int mt_setcursorvisible(int value) {
//     if (value)
//         printf("\033[?25h");
//     else
//         printf("\033[?25l");
//     return 0;
// }

// int mt_delline(void) {
//     printf("\033[2K\r");
//     return 0;
// }