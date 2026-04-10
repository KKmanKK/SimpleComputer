#include "myTerm.h"
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/ioctl.h>
#endif

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
