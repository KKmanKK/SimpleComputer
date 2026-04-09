#include "console.h"

/* Circular scroll buffer: keeps up to INOUT_LINES last entries */
static struct {
    int address;
    int input;   /* 1 = input prompt, 0 = output value */
    int value;
} history[INOUT_LINES];

static int count = 0;  /* number of valid entries (0..INOUT_LINES) */

void printTermClear(void) {
    count = 0;
    for (int i = 0; i < INOUT_LINES; i++) {
        mt_gotoXY(INOUT_ROW + i, INOUT_COL);
        printf("%-*s", BOX_INOUT_W - 2, "");
    }
    fflush(stdout);
}

void printTerm(int address, int input) {
    int value = 0;
    sc_memoryGet(address, &value);

    if (count < INOUT_LINES) {
        /* Buffer not yet full — just append */
        history[count].address = address;
        history[count].input   = input;
        history[count].value   = value;
        count++;
    } else {
        /* Scroll: drop oldest, shift up, add at end */
        for (int i = 0; i < INOUT_LINES - 1; i++)
            history[i] = history[i + 1];
        history[INOUT_LINES - 1].address = address;
        history[INOUT_LINES - 1].input   = input;
        history[INOUT_LINES - 1].value   = value;
    }

    /* Redraw all stored lines */
    for (int i = 0; i < count; i++) {
        mt_gotoXY(INOUT_ROW + i, INOUT_COL);
        if (history[i].input)
            printf("[%3d]: >              ", history[i].address);
        else
            printf("[%3d]: %-10d    ", history[i].address, history[i].value);
    }
}
