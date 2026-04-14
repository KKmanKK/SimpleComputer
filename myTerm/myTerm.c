#include "myTerm.h"
#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/ioctl.h>
#endif

/* Очистка экрана и перемещение курсора в верхний левый угол */
int mt_clrscr(void) {
    printf("\033[H\033[2J");  /* \033[H - курсор в (1,1), \033[2J - очистить экран */
    return 0;
}

/* Удаление текущей строки, где находится курсор */
int mt_delline(void) {
    printf("\033[M");  /* \033[M - удалить строку (DL1) */
    return 0;
}

/* Получение размера терминала (количество строк и столбцов) */
int mt_getscreensize(int *rows, int *cols) {
    if (rows == NULL || cols == NULL)
        return -1;
#ifdef _WIN32
    /* Windows: используем GetConsoleScreenBufferInfo */
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi))
        return -1;
    *cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    *rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
#else
    /* Linux/Unix: используем ioctl с запросом TIOCGWINSZ */
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) < 0)
        return -1;
    *rows = w.ws_row;
    *cols = w.ws_col;
#endif
    if (*rows == 0 || *cols == 0)
        return -1;
    return 0;
}

/* Перемещение курсора в указанную позицию (row - строка, col - столбец) */
int mt_gotoXY(int row, int col) {
    if (row < 0 || col < 0)
        return -1;
    printf("\033[%d;%dH", row, col);
    return 0;
}

/* Установка цвета фона для выводимого текста */
int mt_setbgcolor(enum colors color) {
    if (color < C_BLACK || color > C_DEFAULT)
        return -1;
    printf("\033[4%dm", color);  /* \033[4Xm - где X номер цвета (0-7) */
    return 0;
}

/* Управление видимостью курсора: 1 - показать, 0 - скрыть */
int mt_setcursorvisible(int value) {
    printf(value ? "\033[?25h" : "\033[?25l");  /* ?25h - показать, ?25l - скрыть */
    return 0;
}

/* Сброс всех цветовых настроек к значениям по умолчанию */
int mt_setdefaultcolor(void) {
    printf("\033[0m");  /* сброс всех атрибутов */
    return 0;
}

/* Установка цвета текста (переднего плана) */
int mt_setfgcolor(enum colors color) {
    if (color < C_BLACK || color > C_DEFAULT)
        return -1;
    printf("\033[3%dm", color);  /* \033[3Xm - где X номер цвета (0-7) */
    return 0;
}