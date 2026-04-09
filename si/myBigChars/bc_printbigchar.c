#include "myBigChars.h"

/*
 * bc_printbigchar — render an 8x8 big character on the terminal.
 *
 * symbol[0] encodes rows 1-4 (8 bits per row, LSB = leftmost pixel).
 * symbol[1] encodes rows 5-8 in the same fashion.
 * Bit = 1 → ACS_CKBOARD block ('a' in VT100 alt charset) in fg_color.
 * Bit = 0 → space in bg_color.
 *
 * Returns 0 on success, -1 on invalid arguments.
 */
int bc_printbigchar(int symbol[2], int x, int y,
                    enum colors fg_color, enum colors bg_color) {
    if (x < 0 || y < 0) return -1;

    /* Enter alternate charset for ACS_CKBOARD ('a') */
    printf("\033[%d;%dH\033(0", x, y);

    for (int half = 0; half < 2; half++) {
        int word = symbol[half];
        for (int row = 0; row < 4; row++) {
            int byte = word & 0xff;
            for (int col = 0; col < 8; col++) {
                if (byte & 1)
                    printf("\033[3%dm\033[4%dma\033[39m\033[49m",
                           fg_color, bg_color);
                else
                    printf("\033[4%dm \033[49m", bg_color);
                byte >>= 1;
            }
            printf("\n\033[%d;%dH", ++x, y);
            word >>= 8;
        }
    }

    printf("\033(B\033[%d;0H", x);
    return 0;
}
