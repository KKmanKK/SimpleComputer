#include "console.h"

/*
 * printBigChars — render the current accumulator value as 4 large hex digits
 * using the font loaded from file (g_font).
 *
 * Sign marker (font code 16) is shown at col 2 if the sign bit is set.
 * Digits are placed right-to-left starting at BIGCHAR_COL.
 */
void printBigChars(void) {
    int value;
    sc_accumulatorGet(&value);

    /* Show '+' if the sign/overflow bit is set */
    if ((value >> 16) & 1)
        bc_printbigchar(g_font[16], BIGCHAR_ROW, 2, C_GREEN, C_DEFAULT);

    /* Format lower 16 bits as 4 uppercase hex digits */
    char digits[5];
    snprintf(digits, sizeof(digits), "%04X", value & 0xFFFF);

    /* Render digits right-to-left starting at BIGCHAR_COL */
    int col = BIGCHAR_COL;
    for (int i = 3; i >= 0; i--) {
        int idx;
        char c = digits[i];
        if (c >= '0' && c <= '9') idx = c - '0';
        else if (c >= 'A' && c <= 'F') idx = 10 + (c - 'A');
        else continue;

        bc_printbigchar(g_font[idx], BIGCHAR_ROW, col, C_WHITE, C_DEFAULT);
        col -= 8;
    }
}
