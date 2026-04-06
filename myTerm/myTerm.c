#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "../include/myTerm.h"

int mt_clrscr(void) {
    printf("\033[H\033[J");
    return 0;
}

int mt_gotoXY(int x, int y) {
    if (x < 0 || y < 0) return -1;
    printf("\033[%d;%dH", y, x);
    return 0;
}

int mt_getscreensize(int *rows, int *cols) {
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1)
        return -1;
    if (rows) *rows = ws.ws_row;
    if (cols) *cols = ws.ws_col;
    return 0;
}

int mt_setfgcolor(enum colors color) {
    if (color < 0 || color > 7) return -1;
    printf("\033[3%dm", color);
    return 0;
}

int mt_setbgcolor(enum colors color) {
    if (color < 0 || color > 7) return -1;
    printf("\033[4%dm", color);
    return 0;
}

int mt_setdefaultcolor(void) {
    printf("\033[0m");
    return 0;
}

int mt_setcursorvisible(int value) {
    if (value)
        printf("\033[?25h");
    else
        printf("\033[?25l");
    return 0;
}

int mt_delline(void) {
    printf("\033[2K\r");
    return 0;
}