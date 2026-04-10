#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include "mySimpleComputer.h"
#include "console.h"

/* ── Helpers ─────────────────────────────────────────────────────────────── */

/* Print a one-line status/prompt at PROMPT_ROW and clear it afterwards.
   Returns the number of characters the user typed into `buf` (canonical). */
static int promptLine(const char *label, char *buf, int bufsz) {
    /* Switch to canonical for comfortable text entry */
    rk_mytermregime(1, 0, 0, 1, 1);
    mt_setcursorvisible(1);

    mt_gotoXY(PROMPT_ROW, 1);
    printf("%-*s", MIN_COLS, "");  /* clear the row */
    mt_gotoXY(PROMPT_ROW, 1);
    printf("%s", label);
    fflush(stdout);

    buf[0] = '\0';
    if (fgets(buf, bufsz, stdin)) {
        /* strip trailing newline */
        int n = (int)strlen(buf);
        if (n > 0 && buf[n - 1] == '\n') buf[n - 1] = '\0';
    }

    /* Clear prompt row */
    mt_gotoXY(PROMPT_ROW, 1);
    printf("%-*s", MIN_COLS, "");
    fflush(stdout);

    /* Back to raw mode */
    rk_mytermregime(0, 0, 1, 0, 1);
    mt_setcursorvisible(0);
    return (int)strlen(buf);
}

/* ── Main ────────────────────────────────────────────────────────────────── */

int main(int argc, char *argv[]) {
    /* ── 1. Load font file ────────────────────────────────────────────── */
    const char *font_file = (argc > 1) ? argv[1] : "font.bin";

    int fd = open(font_file, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "Error: cannot open font file '%s': %s\n",
                font_file, strerror(errno));
        return 1;
    }

    int font_count = 0;
    if (bc_bigcharread(fd, (int *)g_font, FONT_SIZE, &font_count) != 0
            || font_count != FONT_SIZE) {
        fprintf(stderr,
                "Error: failed to read font from '%s' "
                "(read %d/%d symbols)\n",
                font_file, font_count, FONT_SIZE);
        close(fd);
        return 1;
    }
    close(fd);

    /* ── 2. Verify stdout is a terminal ──────────────────────────────── */
    if (!isatty(STDOUT_FILENO)) {
        fprintf(stderr, "Error: stdout is not a terminal\n");
        return 1;
    }

    /* ── 3. Check terminal size ───────────────────────────────────────── */
    int rows, cols;
    if (mt_getscreensize(&rows, &cols) != 0) {
        fprintf(stderr, "Error: cannot determine terminal size\n");
        return 1;
    }
    if (rows < MIN_ROWS || cols < MIN_COLS) {
        fprintf(stderr,
            "Error: terminal too small (%dx%d), need at least %dx%d\n",
            cols, rows, MIN_COLS, MIN_ROWS);
        return 1;
    }

    /* ── 4. Init subsystems ──────────────────────────────────────────── */
    sc_memoryInit();
    sc_accumulatorInit();
    sc_icounterInit();
    sc_regInit();
    g_selected = 0;

    /* Start with clock stopped — user presses 'r' to run */
    sc_regSet(FLAG_CLOCK_PULSES_IGNORED, 1);

    /* ── 5. Initial draw ─────────────────────────────────────────────── */
    mt_clrscr();
    mt_setcursorvisible(0);
    drawBoxes();
    refreshAll();

    /* ── 6. Register signal handlers and start timer ─────────────────── */
    rk_mytermsave();
    rk_mytermregime(0, 0, 1, 0, 1); /* raw, VMIN=1, no echo, SIGINT on */

    signal(SIGALRM, IRC);   /* clock pulse → IRC */
    signal(SIGUSR1, IRC);   /* Reset button → IRC */
    signal(SIGUSR2, IRC);   /* forced single step → IRC */

    struct itimerval timer;
    timer.it_value.tv_sec     = 0;
    timer.it_value.tv_usec    = 500000;  /* first tick after 0.5s */
    timer.it_interval.tv_sec  = 0;
    timer.it_interval.tv_usec = 500000;  /* then every 0.5s */
    setitimer(ITIMER_REAL, &timer, NULL);

    /* ── 7. Interactive loop ─────────────────────────────────────────── */
    char filename[256] = "memory.bin";

    while (1) {
        /* While the model is running, block here and let SIGALRM drive CU.
           No interactive keys are processed until the model halts. */
        {
            int running = 0;
            sc_regGet(FLAG_CLOCK_PULSES_IGNORED, &running);
            if (!running) {
                pause();   /* sleep until next signal; IRC/CU will run */
                continue;
            }
        }

        int old = g_selected;

        enum keys key;
        if (rk_readkey(&key) != 0)
            continue;

        switch (key) {

            /* ── Cursor movement ─────────────────────────────────────── */
            case KEY_UP:
            case KEY_DOWN:
            case KEY_LEFT:
            case KEY_RIGHT:
                printCell(old, C_DEFAULT, C_DEFAULT);
                moveCursor(key);
                printCell(g_selected, C_BLACK, C_WHITE);
                refreshSelected();
                break;

            /* ── Edit current cell InPlace ───────────────────────────── */
            case KEY_ENTER:
                mt_setcursorvisible(1);
                if (editCellInPlace(g_selected) == 0) {
                    printCell(g_selected, C_BLACK, C_WHITE);
                    refreshSelected();
                    printCommand();
                } else {
                    printCell(g_selected, C_BLACK, C_WHITE);
                }
                mt_setcursorvisible(0);
                fflush(stdout);
                break;

            /* ── F5: edit accumulator InPlace ────────────────────────── */
            case KEY_F5:
                mt_setcursorvisible(1);
                if (editAccumulatorInPlace() == 0) {
                    printAccumulator();
                    printBigChars();
                }
                mt_setcursorvisible(0);
                fflush(stdout);
                break;

            /* ── F6: edit instruction counter InPlace ────────────────── */
            case KEY_F6:
                mt_setcursorvisible(1);
                if (editICInPlace() == 0) {
                    printCounters();
                    printCommand();
                }
                mt_setcursorvisible(0);
                fflush(stdout);
                break;

            /* ── r: run — clear HALT flag; main loop switches to pause()
                          so no interactive keys are processed while running ── */
            case KEY_R:
                sc_regSet(FLAG_CLOCK_PULSES_IGNORED, 0);
                printFlags();
                fflush(stdout);
                break;  /* next iteration detects running→pause() */

            /* ── s: single step via IRC (no FLAG_T check) ───────────── */
            case KEY_S:
                raise(SIGUSR2);   /* IRC(SIGUSR2) calls CU() unconditionally */
                break;

            /* ── t: save memory to file ──────────────────────────────── */
            case KEY_T: {
                char buf[256];
                mt_setcursorvisible(1);
                if (promptLine("Save file: ", buf, sizeof(buf)) > 0)
                    strncpy(filename, buf, sizeof(filename) - 1);
                if (sc_memorySave(filename) != 0) {
                    promptLine("Error saving file. Press ENTER.", buf, sizeof(buf));
                }
                mt_setcursorvisible(0);
                break;
            }

            /* ── l: load memory from file ────────────────────────────── */
            case KEY_L: {
                char buf[256];
                mt_setcursorvisible(1);
                if (promptLine("Load file: ", buf, sizeof(buf)) > 0)
                    strncpy(filename, buf, sizeof(filename) - 1);
                if (sc_memoryLoad(filename) == 0) {
                    refreshAll();
                } else {
                    promptLine("Error loading file. Press ENTER.", buf, sizeof(buf));
                }
                mt_setcursorvisible(0);
                break;
            }

            /* ── i: reset via SIGUSR1 → IRC handles it ───────────────── */
            case KEY_I:
                raise(SIGUSR1);
                break;

            /* ── ESC: exit ───────────────────────────────────────────── */
            case KEY_ESCAPE:
                goto done;

            default:
                break;
        }
    }

done:
    /* Stop the timer before restoring terminal */
    timer.it_value.tv_sec     = 0;
    timer.it_value.tv_usec    = 0;
    timer.it_interval.tv_sec  = 0;
    timer.it_interval.tv_usec = 0;
    setitimer(ITIMER_REAL, &timer, NULL);

    rk_mytermrestore();
    mt_setcursorvisible(1);
    mt_gotoXY(MIN_ROWS + 1, 1);
    mt_setdefaultcolor();
    printf("\n");
    return 0;
}
