#include "console.h"

void IRC(int signum) {
    if (signum == SIGUSR1) {
        sc_memoryInit();
        sc_accumulatorInit();
        sc_icounterInit();
        sc_regInit();
        sc_regSet(FLAG_CLOCK_PULSES_IGNORED, 1);
        g_selected = 0;
        mt_clrscr();
        drawBoxes();
        refreshAll();
        return;
    }

    if (signum == SIGALRM) {
        int flag = 0;
        sc_regGet(FLAG_CLOCK_PULSES_IGNORED, &flag);
        if (flag) return;
        CU();
        return;
    }

    if (signum == SIGUSR2) {
        CU();
        return;
    }
}