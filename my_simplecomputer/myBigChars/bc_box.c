#include "myBigChars.h"

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
