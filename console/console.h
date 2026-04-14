#ifndef __GUARD_CONSOLE_H
#define __GUARD_CONSOLE_H

#include <stdio.h>
#include <signal.h>
#include <sys/time.h>
#include "mySimpleComputer.h"
#include "myTerm.h"
#include "myBigChars.h"
#include "myReadKey.h"

/* Row below all boxes – used for status/prompt messages */
#define PROMPT_ROW  (MIN_ROWS + 1)

/* ── Terminal size requirements ──────────────────────────────────────────── */

#define MIN_ROWS  35
#define MIN_COLS  92

/* ── Font ────────────────────────────────────────────────────────────────── */

#define FONT_SIZE 18   /* symbols in font file: 0-9, A-F, +, 1 */

/* Globally loaded font (from font.bin or command-line argument).
   Index = font code: 0-9→digits, 10-15→A-F, 16→'+', 17→'1' duplicate. */
extern int g_font[FONT_SIZE][2];

/* Index of the currently selected (edited) memory cell. */
extern int g_selected;

/* ── Layout constants ────────────────────────────────────────────────────── */

/*
 * Box positions are given as (row, col, height, width).
 * Content positions are 1 row/col inside the box border.
 *
 *  Cols  1-62 : left panel  (memory, IN/OUT, big-accumulator)
 *  Cols 64-91 : right panel (acc, IC, decoded, command, flags, big-cell)
 */

/* ── Left panel ─────────────────────────────────────────────────────────── */

/* Memory block: 128 cells, 10/row = 13 content rows */
#define MEM_ROW_ORIGIN    2    /* first content row (inside box) */
#define MEM_COL_ORIGIN    2    /* first content col */
#define MEM_CELLS_PER_ROW 10
#define MEM_CELL_WIDTH    6    /* "+XXXX " */

/* Box: row 1, col 1, height 15 (1 top + 13 content + 1 bottom), width 62 */
#define BOX_MEM_ROW    1
#define BOX_MEM_COL    1
#define BOX_MEM_H      15
#define BOX_MEM_W      62

/* IN-OUT block (4 scrolling lines) */
#define INOUT_ROW   17         /* first content row (box top = row 16) */
#define INOUT_COL    2
#define INOUT_LINES  4

/* Box: row 16, col 1, height 6, width 62 */
#define BOX_INOUT_ROW  16
#define BOX_INOUT_COL   1
#define BOX_INOUT_H     6
#define BOX_INOUT_W    62

/* Big-accumulator display: 4 large hex digits (8×8 each) */
/* sign at col 2, digits right-to-left at col 34, 26, 18, 10 */
#define BIGCHAR_ROW  26        /* top row of 8-row symbol area (box top = 25) */
#define BIGCHAR_COL  34        /* rightmost digit column */

/* Box: row 25, col 1, height 10, width 44 */
#define BOX_BIGCHAR_ROW  25
#define BOX_BIGCHAR_COL   1
#define BOX_BIGCHAR_H    10
#define BOX_BIGCHAR_W    44

/* ── Right panel (content start col = RP_COL) ───────────────────────────── */

#define RP_COL  65             /* first content column inside right-panel boxes */
                               /* box left border = col 64, width = 28 */
#define BOX_RP_COL  64
#define BOX_RP_W    28

/* Accumulator: DEC + HEX (box top = row 1) */
#define ACC_ROW   2

/* Box: row 1, col 64, height 4, width 28 */
#define BOX_ACC_ROW  1
#define BOX_ACC_H    4

/* Instruction counter (box top = row 5) */
#define IC_ROW   6

/* Box: row 5, col 64, height 3, width 28 */
#define BOX_IC_ROW  5
#define BOX_IC_H    3

/* Decoded format of selected cell: DEC, OCT, HEX, BIN (box top = row 8) */
#define DECODED_ROW   9

/* Box: row 8, col 64, height 6, width 28 */
#define BOX_DECODED_ROW  8
#define BOX_DECODED_H    6

/* Current instruction at IC (box top = row 14) */
#define CMD_ROW  15

/* Box: row 14, col 64, height 3, width 28 */
#define BOX_CMD_ROW  14
#define BOX_CMD_H     3

/* Flags register (box top = row 17) */
#define FLAGS_ROW  18

/* Box: row 17, col 64, height 3, width 28 */
#define BOX_FLAGS_ROW  17
#define BOX_FLAGS_H     3

/* Keys hint box (rows 20-24, right panel) */
#define BOX_KEYS_ROW  20
#define BOX_KEYS_H     5
#define KEYS_ROW1     21   /* first hint line  */
#define KEYS_ROW2     22   /* second hint line */
#define KEYS_ROW3     23   /* third hint line  */

/* Big-cell display: enlarged value of the selected cell.
   Placed to the right of the big-accumulator box (same rows, different cols).
   sign at col 46, digits right-to-left at col 78, 70, 62, 54             */
#define BIGCELL_ROW       26   /* top row of 8-row symbol area (box top = 25) */
#define BIGCELL_COL       78   /* rightmost digit column */
#define BIGCELL_SIGN_COL  46   /* '+' sign column */

/* Box: row 25, col 45, height 10, width 47 — side-by-side with BOX_BIGCHAR */
#define BOX_BIGCELL_ROW  25
#define BOX_BIGCELL_COL  45
#define BOX_BIGCELL_H    10
#define BOX_BIGCELL_W    47

/* ── Function prototypes ─────────────────────────────────────────────────── */

void drawBoxes(void);
void printCell(int address, enum colors fg, enum colors bg);
void printFlags(void);
void printDecodedCommand(int value);
void printAccumulator(void);
void printCounters(void);
void printTerm(int address, int input);
void printTermClear(void);
void printCommand(void);
void printBigChars(void);
void printBigCell(void);
int promptLine(const char *label, char *buf, int bufsz);
/* Cursor movement */
void moveCursor(enum keys dir);

/* InPlace editors — return 0 on confirm, -1 on cancel */
int editCellInPlace(int address);
int editAccumulatorInPlace(void);
int editICInPlace(void);

/* Screen refresh */
void refreshSelected(void);
void refreshAll(void);

/* CPU subsystems */
int  ALU(int command, int operand);
void CU(void);
void IRC(int signum);

#endif /* __GUARD_CONSOLE_H */