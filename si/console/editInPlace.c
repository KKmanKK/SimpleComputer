#include "console.h"
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>

/*
 * readField — generic InPlace field editor.
 *
 * Positions cursor at (row, col), clears `width` characters, then reads
 * input in raw mode (no echo from terminal; each accepted character is
 * echoed manually so it appears right at the cursor position).
 *
 * allowed : string of accepted characters (NULL = all printable)
 * maxlen  : maximum number of input characters (output buffer must be maxlen+1)
 * out     : buffer where the typed string is stored
 *
 * Returns  0 on ENTER (confirm), -1 on ESC or error (cancel).
 */
static int readField(int row, int col, int width, int maxlen,
                     const char *allowed, char *out) {
    /* Clear the field and move cursor to start */
    mt_gotoXY(row, col);
    for (int i = 0; i < width; i++)
        putchar(' ');
    mt_gotoXY(row, col);
    fflush(stdout);

    /* Raw mode: non-canonical, no echo, VMIN=1, VTIME=0 */
    rk_mytermregime(0, 0, 1, 0, 0);

    int len = 0;
    int ret = -1;

    while (1) {
        unsigned char c;
        ssize_t n = read(STDIN_FILENO, &c, 1);
        if (n <= 0)
            break;

        if (c == '\n' || c == '\r') {
            ret = 0;
            break;
        }

        if (c == 0x1B) {
            /* Drain any pending escape sequence bytes (e.g. arrow key tail) */
            rk_mytermregime(0, 1, 0, 0, 0);      /* VTIME=0.1s, VMIN=0 */
            unsigned char tmp[8];
            read(STDIN_FILENO, tmp, sizeof(tmp)); /* ignore */
            rk_mytermregime(0, 0, 1, 0, 0);      /* restore VMIN=1 */
            ret = -1;
            break;
        }

        if ((c == 127 || c == '\b') && len > 0) {
            len--;
            write(STDOUT_FILENO, "\b \b", 3);
            continue;
        }

        if (len >= maxlen)
            continue;

        /* Check against allowed character set */
        int ok;
        if (allowed == NULL) {
            ok = isprint((unsigned char)c);
        } else {
            ok = 0;
            for (const char *p = allowed; *p; p++) {
                if (*p == (char)c) { ok = 1; break; }
            }
        }
        if (!ok)
            continue;

        write(STDOUT_FILENO, &c, 1); /* manual echo at cursor position */
        out[len++] = (char)c;
    }

    out[len] = '\0';

    /* Restore raw mode used by the main loop: no echo, SIGINT on */
    rk_mytermregime(0, 0, 1, 0, 1);
    return ret;
}

/* ── Public editors ──────────────────────────────────────────────────────── */

/*
 * editCellInPlace — edit memory cell `address` at its screen position.
 * Accepts format: [+-][0-9A-Fa-f]{1,4}  (same as display format).
 */
int editCellInPlace(int address) {
    int row = MEM_ROW_ORIGIN + (address / MEM_CELLS_PER_ROW);
    int col = MEM_COL_ORIGIN + (address % MEM_CELLS_PER_ROW) * MEM_CELL_WIDTH;

    char buf[8];
    if (readField(row, col, MEM_CELL_WIDTH - 1, 5,
                  "+-0123456789ABCDEFabcdef", buf) != 0)
        return -1;

    if (buf[0] == '\0')
        return 0;

    int  sign = 1;
    const char *hex = buf;
    if      (buf[0] == '+') { sign =  1; hex = buf + 1; }
    else if (buf[0] == '-') { sign = -1; hex = buf + 1; }

    if (*hex == '\0')
        return 0;

    long raw = strtol(hex, NULL, 16);
    int  val = (int)(sign < 0 ? -raw : raw);

    if (val < MEMORY_MIN_VALUE) val = MEMORY_MIN_VALUE;
    if (val > MEMORY_MAX_VALUE) val = MEMORY_MAX_VALUE;

    sc_memorySet(address, val);
    return 0;
}

/*
 * editAccumulatorInPlace — edit accumulator at its "DEC:" field.
 * Accepts signed decimal in range ACC_MIN_VALUE..ACC_MAX_VALUE.
 */
int editAccumulatorInPlace(void) {
    /* Position after "DEC: " label (5 chars) */
    char buf[12];
    if (readField(ACC_ROW, RP_COL + 5, 10, 6,
                  "+-0123456789", buf) != 0)
        return -1;

    if (buf[0] == '\0')
        return 0;

    int val = (int)strtol(buf, NULL, 10);
    if (val < ACC_MIN_VALUE) val = ACC_MIN_VALUE;
    if (val > ACC_MAX_VALUE) val = ACC_MAX_VALUE;

    sc_accumulatorSet(val);
    return 0;
}

/*
 * editICInPlace — edit instruction counter at its "IC:" field.
 * Accepts decimal address in range 0..MEMORY_SIZE-1.
 */
int editICInPlace(void) {
    /* Position after "IC: " label (4 chars) */
    char buf[6];
    if (readField(IC_ROW, RP_COL + 4, 6, 3,
                  "0123456789", buf) != 0)
        return -1;

    if (buf[0] == '\0')
        return 0;

    int val = (int)strtol(buf, NULL, 10);
    if (val < 0)           val = 0;
    if (val >= MEMORY_SIZE) val = MEMORY_SIZE - 1;

    sc_icounterSet(val);
    return 0;
}
