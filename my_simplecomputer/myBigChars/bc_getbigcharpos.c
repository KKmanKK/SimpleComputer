#include "myBigChars.h"

/*
 * bc_getbigcharpos — read a single pixel from an 8x8 big-char bitmap.
 *
 * big   — two-int bitmap array
 * x     — row (1-8)
 * y     — column (1-8)
 * value — output: 1 if the pixel is set, 0 otherwise
 *
 * Returns 0 on success, -1 on invalid arguments.
 */
int bc_getbigcharpos(int *big, int x, int y, int *value) {
    x--; y--;
    if (x < 0 || x > 7 || y < 0 || y > 7 || !value) return -1;

    int index = (x < 4) ? 0 : 1;
    int bit   = y + (x % 4) * 8;

    *value = (big[index] >> bit) & 0x1;

    return 0;
}
