#include "console.h"
#include <signal.h>

/*
 * IRC — Interrupt Request Controller.
 *
 * Handles three signals:
 *   SIGALRM — from the system timer (clock pulse generator).
 *             If FLAG_CLOCK_PULSES_IGNORED is clear, runs one CU tick.
 *
 *   SIGUSR1 — from the Reset button (key 'i').
 *             Resets the entire machine state and redraws the screen.
 *
 *   SIGUSR2 — from a single-step request (key 's' in interactive mode).
 *             Runs exactly one CU tick WITHOUT checking FLAG_CLOCK_PULSES_IGNORED.
 */
void IRC(int signum) {
    if (signum == SIGUSR1) {
        /* ── Reset ─────────────────────────────────────────────────── */
        sc_memoryInit();
        sc_accumulatorInit();
        sc_icounterInit();
        sc_regInit();
        /* FLAG_CLOCK_PULSES_IGNORED is cleared by sc_regInit,
           so clock resumes — stop it again until user presses r */
        sc_regSet(FLAG_CLOCK_PULSES_IGNORED, 1);
        g_selected = 0;
        mt_clrscr();
        drawBoxes();
        refreshAll();
        return;
    }

    if (signum == SIGALRM) {
        /* ── Clock pulse ────────────────────────────────────────────── */
        int flag = 0;
        sc_regGet(FLAG_CLOCK_PULSES_IGNORED, &flag);
        if (flag)
            return;   /* clock is stopped — ignore this tick */
        CU();
        return;
    }

    if (signum == SIGUSR2) {
        /* ── Forced single step (key 's') — no FLAG_T check ────────── */
        CU();
        return;
    }
}
