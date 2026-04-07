#include "myTerm.h"

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