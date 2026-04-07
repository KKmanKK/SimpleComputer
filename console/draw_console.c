#include "console.h"

int draw_console(int *rows, int *cols) {    
    mt_getscreensize(rows, cols);
    bc_box(1, 1, 12, 61);
    bc_box(1, 63, 3, 30);
    bc_box(4, 63, 3, 30);
    bc_box(7, 63, 3, 30);
    bc_box(10, 63, 3, 30);
    bc_box(13, 1, 10, 42);
    bc_box(13, 44, 10, 49);
    bc_box(23, 1, 7, 92);
    mt_gotoXY (29, 1); printf("Memory");
    mt_gotoXY (72, 1); printf("accumulator");
    mt_gotoXY (70, 4); printf("instructionCounter");
    mt_gotoXY (73, 7); printf("Operation");
    mt_gotoXY (75, 10); printf("Flags");
    mt_gotoXY (50, 13); printf("Keys:");
    mt_gotoXY (46, 14); printf(" l - load");
    mt_gotoXY (46, 15); printf(" s - save");
    mt_gotoXY (46, 16); printf(" r - run");
    mt_gotoXY (46, 17); printf(" t - step");
    mt_gotoXY (46, 18); printf(" i - reset");
    mt_gotoXY (46, 19); printf(" F5 - accumulator");
    mt_gotoXY (46, 20); printf(" F6 - instructionCounter");
    mt_gotoXY (46, 21); printf(" Esc - exit");
    mt_gotoXY (0, 29);
    return 0;
}