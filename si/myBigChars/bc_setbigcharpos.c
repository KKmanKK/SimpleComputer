#include "myBigChars.h"

/*
 * bc_setbigcharpos — set or clear a single pixel in an 8x8 big-char bitmap.
 *
 * big     — two-int bitmap array
 * x       — row (1-8)
 * y       — column (1-8)
 * value   — 1 to set the pixel, 0 to clear it
 *
 * Returns 0 on success, -1 on invalid arguments.
 */
int bc_setbigcharpos(int *big, int x, int y, int value) {
    x--; y--;
    if (x < 0 || x > 7 || y < 0 || y > 7 || value < 0 || value > 1) return -1;

    /* Rows 1-4 → big[0], rows 5-8 → big[1] */
    int index = (x < 4) ? 0 : 1;
    int bit   = y + (x % 4) * 8;

    if (value)
        big[index] |=  (1 << bit);
    else
        big[index] &= ~(1 << bit);

    return 0;
}
