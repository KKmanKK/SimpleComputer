#include "myBigChars.h"
#include <stdio.h>
#include <stdlib.h>

/* Global font data */
// unsigned int bc[BIGCHAR_COUNT][2];

/*
 * bc_strlen — count the number of Unicode characters in a UTF-8 string.
 *
 * UTF-8 encoding rules:
 *   0xxxxxxx              — 1-byte sequence (U+0000 .. U+007F)
 *   110xxxxx 10xxxxxx     — 2-byte sequence (U+0080 .. U+07FF)
 *   1110xxxx 10xxxxxx×2   — 3-byte sequence (U+0800 .. U+FFFF)
 *   11110xxx 10xxxxxx×3   — 4-byte sequence (U+10000 .. U+10FFFF)
 *
 * Returns 0 if str is NULL or any byte sequence cannot be decoded.
 */
int bc_strlen(char *str) {
    if (!str) return 0;

    unsigned char *p = (unsigned char *)str;
    int count = 0;

    while (*p) {
        unsigned char c = *p;
        int bytes;

        if      (c < 0x80)            bytes = 1;  /* 0xxxxxxx */
        else if ((c & 0xE0) == 0xC0)  bytes = 2;  /* 110xxxxx */
        else if ((c & 0xF0) == 0xE0)  bytes = 3;  /* 1110xxxx */
        else if ((c & 0xF8) == 0xF0)  bytes = 4;  /* 11110xxx */
        else return 0;                             /* invalid start byte */

        /* Validate continuation bytes (must be 10xxxxxx) */
        for (int i = 1; i < bytes; i++) {
            if ((p[i] & 0xC0) != 0x80) return 0;
        }

        p += bytes;
        count++;
    }

    return count;
}

/*
 * bc_printA — print a string using VT100 alternate charset (pseudo-graphics).
 *
 * str — the string to print; each character is mapped to the ACS symbol table.
 *
 * Returns 0 on success, -1 if str is NULL.
 */
int bc_printA(char *str) {
    if (str == NULL) return -1;

    printf("\033(0%s\033(B", str);
    return 0;
}

/*
 * bc_box — draw a colored pseudo-graphic box with an optional UTF-8 header.
 *
 * x1, y1       — top-left corner (row, col)
 * x2           — height in rows
 * y2           — width in columns
 * box_fg/bg    — color of the border characters
 * header       — if non-NULL and valid UTF-8 that fits the top border,
 *                printed centered there in header_fg/header_bg colors
 *
 * Returns 0 on success, -1 on invalid arguments.
 */
int bc_box(int x1, int y1, int x2, int y2,
           enum colors box_fg, enum colors box_bg,
           char *header,
           enum colors header_fg, enum colors header_bg) {

    if (x1 < 0 || y1 < 0 || x2 < 2 || y2 < 2) return -1;

    int inner = y2 - 2;   /* columns available between the two corner chars */

    /* Determine if the header fits */
    int hdr_chars = 0;
    int has_header = 0;
    if (header) {
        hdr_chars = bc_strlen(header);
        if (hdr_chars > 0 && hdr_chars < inner) has_header = 1;
    }

    /* ── Top border ─────────────────────────────────────────────────────── */
    printf("\033[%d;%dH\033[3%dm\033[4%dm\033(0l", x1, y1, box_fg, box_bg);

    if (has_header) {
        int left_q  = (inner - hdr_chars) / 2;
        int right_q = inner - hdr_chars - left_q;

        for (int i = 0; i < left_q;  i++) printf("q");
        /* Switch to normal charset and header colors for the label */
        printf("\033(B\033[3%dm\033[4%dm%s", header_fg, header_bg, header);
        /* Restore box colors and alternate charset */
        printf("\033[3%dm\033[4%dm\033(0", box_fg, box_bg);
        for (int i = 0; i < right_q; i++) printf("q");
    } else {
        for (int i = 0; i < inner; i++) printf("q");
    }
    printf("k");

    /* ── Side borders ───────────────────────────────────────────────────── */
    for (int row = 1; row < x2 - 1; row++) {
        printf("\033[%d;%dH", x1 + row, y1);
        printf("x");
        for (int j = 0; j < inner; j++) printf(" ");
        printf("x");
    }

    /* ── Bottom border ──────────────────────────────────────────────────── */
    printf("\033[%d;%dH", x1 + x2 - 1, y1);
    printf("m");
    for (int i = 0; i < inner; i++) printf("q");
    printf("j");

    /* Exit alternate charset and reset colors */
    printf("\033(B\033[39m\033[49m");

    return 0;
}

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

/*
 * bc_bigcharwrite — write big-char bitmaps to a file descriptor.
 *
 * fd    — open file descriptor for writing
 * big   — array of two-int bitmaps (count symbols)
 * count — number of symbols to write
 *
 * Returns 0 on success, -1 on error or invalid arguments.
 */
int bc_bigcharwrite(int fd, int *big, int count) {
    if (fd < 0 || count < 0) return -1;

    int result = write(fd, big, count * sizeof(int) * 2);
    return (result == count * (int)(sizeof(int) * 2)) ? 0 : -1;
}

/*
 * bc_bigcharread — read big-char bitmaps from a file descriptor.
 *
 * fd         — open file descriptor for reading
 * big        — buffer to store the bitmaps (must hold need_count symbols)
 * need_count — maximum number of symbols to read
 * count      — output: actual number of symbols read
 *
 * Returns 0 on success, -1 on error or invalid arguments.
 */
int bc_bigcharread(int fd, int *big, int need_count, int *count) {
    if (fd < 0 || need_count <= 0 || !count) return -1;

    int read_bytes = read(fd, big, need_count * sizeof(int) * 2);
    *count = read_bytes / (int)(sizeof(int) * 2);

    return (read_bytes >= 0) ? 0 : -1;
}