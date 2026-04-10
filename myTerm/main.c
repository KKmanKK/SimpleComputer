#include <stdio.h>
#include <unistd.h>
#include "myTerm.h"

int main(void) {
    int rows, cols;

    /* Clear screen */
    mt_clrscr();

    /* Show terminal size */
    if (mt_getscreensize(&rows, &cols) == 0) {
        mt_gotoXY(1, 1);
        printf("Terminal size: %d rows x %d cols\n", rows, cols);
    }

    /* Row 5, col 10: name — red on black */
    mt_gotoXY(5, 10);
    mt_setfgcolor(C_RED);
    mt_setbgcolor(C_BLACK);
    write(STDOUT_FILENO, "Daniil", 6);
    mt_setdefaultcolor();

    /* Row 6, col 8: group — green on white */
    mt_gotoXY(6, 8);
    mt_setfgcolor(C_GREEN);
    mt_setbgcolor(C_WHITE);
    write(STDOUT_FILENO, "ZP-31", 5);
    mt_setdefaultcolor();

    /* Hide cursor, wait, restore */
    mt_gotoXY(8, 1);
    printf("Cursor hidden for 1 second...\n");
    mt_setcursorvisible(0);
    sleep(1);
    mt_setcursorvisible(1);

    /* Demo delete line */
    mt_gotoXY(9, 1);
    printf("This line will be deleted.");
    mt_gotoXY(9, 1);
    mt_delline();

    /* Move to row 10, reset colors */
    mt_gotoXY(10, 1);
    mt_setdefaultcolor();

    return 0;
}