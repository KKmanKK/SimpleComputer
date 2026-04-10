#include "myTerm.h"
#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/ioctl.h>
#endif

/* Clear screen */
int mt_clrscr(void) {
    /* \033[H  - move cursor to top-left */
    /* \033[2J - erase entire screen     */
    printf("\033[H\033[2J");
    return 0;
}

/* Delete current line */
int mt_delline(void) {
    /* \033[M - delete current line (dl1) */
    printf("\033[M");
    return 0;
}

/* Get terminal size */
int mt_getscreensize(int *rows, int *cols) {
    if (rows == NULL || cols == NULL)
        return -1;
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi))
        return -1;
    *cols = csbi.srWindow.Right  - csbi.srWindow.Left + 1;
    *rows = csbi.srWindow.Bottom - csbi.srWindow.Top  + 1;
    if (*rows == 0 || *cols == 0)
        return -1;
    return 0;
#else
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) < 0)
        return -1;
    if (w.ws_row == 0 || w.ws_col == 0)
        return -1;
    *rows = w.ws_row;
    *cols = w.ws_col;
    return 0;
#endif
}

/* Move cursor to position */
int mt_gotoXY(int row, int col) {
    if (row < 0 || col < 0)
        return -1;
    printf("\033[%d;%dH", row, col);
    return 0;
}

/* Set background color */
int mt_setbgcolor(enum colors color) {
    if (color < C_BLACK || color > C_DEFAULT)
        return -1;
    printf("\033[4%dm", color);
    return 0;
}

/* Set cursor visibility */
int mt_setcursorvisible(int value) {
    /* \033[?25h - show cursor */
    /* \033[?25l - hide cursor */
    printf(value ? "\033[?25h" : "\033[?25l");
    return 0;
}

/* Reset to default colors */
int mt_setdefaultcolor(void) {
    printf("\033[0m");
    return 0;
}

/* Set foreground color */
int mt_setfgcolor(enum colors color) {
    if (color < C_BLACK || color > C_DEFAULT)
        return -1;
    printf("\033[3%dm", color);
    return 0;
}