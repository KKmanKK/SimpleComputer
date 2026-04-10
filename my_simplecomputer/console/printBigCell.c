#include "console.h"

/*
 * font_index — return the g_font index for a hex digit character.
 * Returns -1 for unrecognised characters.
 */
static int font_index(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return 10 + (c - 'A');
    return -1;
}

/*
 * printBigCell — render the value of the currently selected memory cell
 * as 4 large hex digits using the font loaded from file.
 *
 * A '+' marker (font code 16) is shown to the left if the cell holds
 * a command (sign bit set).  Digits are placed right-to-left:
 *   BIGCELL_COL, BIGCELL_COL-8, BIGCELL_COL-16, BIGCELL_COL-24.
 */
void printBigCell(void) {
    int value = 0;
    sc_memoryGet(g_selected, &value);

    int sign, command, operand;
    sc_commandDecode(value, &sign, &command, &operand);

    /* Show '+' marker for command cells */
    if (sign == 1)
        bc_printbigchar(g_font[16], BIGCELL_ROW, BIGCELL_SIGN_COL,
                        C_GREEN, C_DEFAULT);

    /* Format lower 16 bits as 4 uppercase hex digits */
    char digits[5];
    snprintf(digits, sizeof(digits), "%04X", value & 0xFFFF);

    /* Print digits right-to-left starting at BIGCELL_COL */
    int col = BIGCELL_COL;
    for (int i = 3; i >= 0; i--) {
        int idx = font_index(digits[i]);
        if (idx >= 0)
            bc_printbigchar(g_font[idx], BIGCELL_ROW, col,
                            C_WHITE, C_DEFAULT);
        col -= 8;
    }
}
