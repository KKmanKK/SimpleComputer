#ifndef __GUARD_MYBIGCHARS_H
#define __GUARD_MYBIGCHARS_H

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "myTerm.h"

/* 8x8 bitmap representations of hex digits and letters (two ints each) */
#define BIG_SYMBOL_0 1111638588, 1010975298
#define BIG_SYMBOL_1 303306768, 2081427472
#define BIG_SYMBOL_2 541213244, 2114193424
#define BIG_SYMBOL_3 943866428, 1010975296
#define BIG_SYMBOL_4 606613536, 538976382
#define BIG_SYMBOL_5 1040319102, 1010974784
#define BIG_SYMBOL_6 1040335420, 1010975298
#define BIG_SYMBOL_7 270549118, 134744072
#define BIG_SYMBOL_8 1010975292, 1010975298
#define BIG_SYMBOL_9 1111638588, 1010974844
#define BIG_SYMBOL_PLUS 404226048, 1579134

#define BIG_SYMBOL_A 2118263832, 1111638594
#define BIG_SYMBOL_B 505553438, 2118271522
#define BIG_SYMBOL_C 33702460, 1010975234
#define BIG_SYMBOL_D 1111638590, 1044529730
#define BIG_SYMBOL_E 503448126, 2114060802
#define BIG_SYMBOL_F 503448126, 33686018

/*
 * Count the number of Unicode characters in a UTF-8 string.
 * Returns 0 if str is NULL or contains an invalid byte sequence.
 */
int bc_strlen(char *str);

/* Print a string using VT100 alternate charset (pseudo-graphic symbols). */
int bc_printA(char *str);

/*
 * Draw a pseudo-graphic box.
 *   Top-left corner: (x1, y1).  Height: x2 rows.  Width: y2 cols.
 *   Box characters are drawn in box_fg / box_bg colors.
 *   If header is non-NULL and is valid UTF-8 that fits inside the top border,
 *   it is printed centered there in header_fg / header_bg colors.
 */
int bc_box(int x1, int y1, int x2, int y2,
           enum colors box_fg, enum colors box_bg,
           char *header,
           enum colors header_fg, enum colors header_bg);

/*
 * Render an 8x8 big character at screen position (x, y).
 * Bits 0-7 of symbol[0] → row 1, bits 8-15 → row 2, …, bits 24-31 of symbol[1] → row 8.
 * Bit = 1 → ACS_CKBOARD block in fg_color; bit = 0 → space in bg_color.
 */
int bc_printbigchar(int symbol[2], int x, int y,
                    enum colors fg_color, enum colors bg_color);

/* Set a single pixel (bit) at (x, y) in the 8x8 bitmap big[2]. */
int bc_setbigcharpos(int *big, int x, int y, int value);

/* Read a single pixel value at (x, y) from the 8x8 bitmap big[2]. */
int bc_getbigcharpos(int *big, int x, int y, int *value);

/* Write count big-char bitmaps to file descriptor fd (binary format). */
int bc_bigcharwrite(int fd, int *big, int count);

/*
 * Read up to need_count big-char bitmaps from fd.
 * Stores actual number read in *count (0 on error).
 */
int bc_bigcharread(int fd, int *big, int need_count, int *count);

#endif /* __GUARD_MYBIGCHARS_H */
