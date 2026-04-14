#include "console.h"
#include <fcntl.h>
#include <errno.h>
#include <string.h>  

int main(int argc, char *argv[]) {
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
                "Error: failed to read font from '%s' (read %d/%d symbols)\n",
                font_file, font_count, FONT_SIZE);
        close(fd);
        return 1;
    }
    close(fd);

    if (!isatty(STDOUT_FILENO)) {
        fprintf(stderr, "Error: stdout is not a terminal\n");
        return 1;
    }

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

    sc_memoryInit();
    sc_accumulatorInit();
    sc_icounterInit();
    sc_regInit();
    g_selected = 0;
    sc_regSet(FLAG_CLOCK_PULSES_IGNORED, 1);

    mt_clrscr();
    mt_setcursorvisible(0);
    drawBoxes();
    refreshAll();

    rk_mytermsave();
    rk_mytermregime(0, 0, 1, 0, 1);

    signal(SIGALRM, IRC);
    signal(SIGUSR1, IRC);
    signal(SIGUSR2, IRC);

    struct itimerval timer;
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = 500000;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 500000;
    setitimer(ITIMER_REAL, &timer, NULL);

    char filename[256] = "memory.bin";

    while (1) {
        int running = 0;
        sc_regGet(FLAG_CLOCK_PULSES_IGNORED, &running);
        if (!running) {
            pause();
            continue;
        }

        int old = g_selected;
        enum keys key;
        if (rk_readkey(&key) != 0) continue;

        switch (key) {
            case KEY_UP:
            case KEY_DOWN:
            case KEY_LEFT:
            case KEY_RIGHT:
                printCell(old, C_DEFAULT, C_DEFAULT);
                moveCursor(key);
                printCell(g_selected, C_BLACK, C_WHITE);
                refreshSelected();
                break;

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

            case KEY_F5:
                mt_setcursorvisible(1);
                if (editAccumulatorInPlace() == 0) {
                    printAccumulator();
                    printBigChars();
                }
                mt_setcursorvisible(0);
                fflush(stdout);
                break;

            case KEY_F6:
                mt_setcursorvisible(1);
                if (editICInPlace() == 0) {
                    printCounters();
                    printCommand();
                }
                mt_setcursorvisible(0);
                fflush(stdout);
                break;

            case KEY_R:
                sc_regSet(FLAG_CLOCK_PULSES_IGNORED, 0);
                printFlags();
                fflush(stdout);
                break;

            case KEY_S:
                raise(SIGUSR2);
                break;

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

            case KEY_I:
                raise(SIGUSR1);
                break;

            case KEY_ESCAPE:
                goto done;

            default:
                break;
        }
    }

done:
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 0;
    setitimer(ITIMER_REAL, &timer, NULL);

    rk_mytermrestore();
    mt_setcursorvisible(1);
    mt_gotoXY(MIN_ROWS + 1, 1);
    mt_setdefaultcolor();
    printf("\n");
    return 0;
}