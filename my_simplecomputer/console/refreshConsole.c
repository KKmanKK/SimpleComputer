#include "console.h"

/* Redraw only the panels that depend on the selected cell. */
void refreshSelected(void) {
    int val = 0;
    sc_memoryGet(g_selected, &val);
    printDecodedCommand(val);
    printBigCell();
    fflush(stdout);
}

/* Full screen repaint (called after load, reset, or first draw). */
void refreshAll(void) {
    printTermClear();
    for (int i = 0; i < MEMORY_SIZE; i++) {
        if (i == g_selected)
            printCell(i, C_BLACK, C_WHITE);
        else
            printCell(i, C_DEFAULT, C_DEFAULT);
    }
    printAccumulator();
    printCounters();
    printCommand();
    printFlags();
    printBigChars();
    refreshSelected();
    fflush(stdout);
}
