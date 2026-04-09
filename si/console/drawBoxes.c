#include "console.h"

/*
 * drawBoxes — draw all pseudo-graphic frames with ASCII headers.
 *
 * ASCII headers are used for portability (avoid UTF-8 encoding issues
 * on Windows terminals that may misinterpret multi-byte sequences).
 */
void drawBoxes(void) {
    /* ── Left panel ─────────────────────────────────────────────────── */

    bc_box(BOX_MEM_ROW, BOX_MEM_COL, BOX_MEM_H, BOX_MEM_W,
           C_CYAN, C_DEFAULT,
           "Memory",
           C_YELLOW, C_DEFAULT);

    bc_box(BOX_INOUT_ROW, BOX_INOUT_COL, BOX_INOUT_H, BOX_INOUT_W,
           C_CYAN, C_DEFAULT,
           "IN/OUT",
           C_YELLOW, C_DEFAULT);

    bc_box(BOX_BIGCHAR_ROW, BOX_BIGCHAR_COL, BOX_BIGCHAR_H, BOX_BIGCHAR_W,
           C_CYAN, C_DEFAULT,
           "Accumulator",
           C_YELLOW, C_DEFAULT);

    bc_box(BOX_BIGCELL_ROW, BOX_BIGCELL_COL, BOX_BIGCELL_H, BOX_BIGCELL_W,
           C_CYAN, C_DEFAULT,
           "Cell",
           C_YELLOW, C_DEFAULT);

    /* ── Right panel ────────────────────────────────────────────────── */

    bc_box(BOX_ACC_ROW, BOX_RP_COL, BOX_ACC_H, BOX_RP_W,
           C_CYAN, C_DEFAULT,
           "Accumulator",
           C_YELLOW, C_DEFAULT);

    bc_box(BOX_IC_ROW, BOX_RP_COL, BOX_IC_H, BOX_RP_W,
           C_CYAN, C_DEFAULT,
           "Instruction Counter",
           C_YELLOW, C_DEFAULT);

    bc_box(BOX_DECODED_ROW, BOX_RP_COL, BOX_DECODED_H, BOX_RP_W,
           C_CYAN, C_DEFAULT,
           "Selected Cell (format)",
           C_YELLOW, C_DEFAULT);

    bc_box(BOX_CMD_ROW, BOX_RP_COL, BOX_CMD_H, BOX_RP_W,
           C_CYAN, C_DEFAULT,
           "Command",
           C_YELLOW, C_DEFAULT);

    bc_box(BOX_FLAGS_ROW, BOX_RP_COL, BOX_FLAGS_H, BOX_RP_W,
           C_CYAN, C_DEFAULT,
           "Flags",
           C_YELLOW, C_DEFAULT);

    bc_box(BOX_KEYS_ROW, BOX_RP_COL, BOX_KEYS_H, BOX_RP_W,
           C_CYAN, C_DEFAULT,
           "Keys",
           C_YELLOW, C_DEFAULT);

    mt_setfgcolor(C_WHITE);
    mt_gotoXY(KEYS_ROW1, RP_COL); printf("l-load  t-save  i-reset");
    mt_gotoXY(KEYS_ROW2, RP_COL); printf("r-run   s-step  ESC-quit");
    mt_gotoXY(KEYS_ROW3, RP_COL); printf("F5-acc  F6-ic");
    mt_setdefaultcolor();
}
