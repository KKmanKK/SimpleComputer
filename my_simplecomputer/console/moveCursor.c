#include "console.h"

/*
 * Cells per row in each row:
 *   rows 0..11 : MEM_CELLS_PER_ROW  (10)
 *   row 12     : MEMORY_SIZE - 12*10 = 8
 */
static int cells_in_row(int row) {
    int last = (row + 1) * MEM_CELLS_PER_ROW;
    return (last > MEMORY_SIZE) ? MEMORY_SIZE - row * MEM_CELLS_PER_ROW
                                 : MEM_CELLS_PER_ROW;
}

/*
 * moveCursor — move g_selected cyclically.
 *
 * LEFT/RIGHT: wrap within the current row.
 * UP/DOWN   : wrap within the current column (same column index).
 *             If the target row has fewer columns than the current col,
 *             the cursor lands on the last valid cell of that row.
 */
void moveCursor(enum keys dir) {
    int total_rows = (MEMORY_SIZE + MEM_CELLS_PER_ROW - 1) / MEM_CELLS_PER_ROW;
    int row = g_selected / MEM_CELLS_PER_ROW;
    int col = g_selected % MEM_CELLS_PER_ROW;

    switch (dir) {
        case KEY_LEFT:
            col = (col == 0) ? cells_in_row(row) - 1 : col - 1;
            break;

        case KEY_RIGHT:
            col = (col + 1) % cells_in_row(row);
            break;

        case KEY_UP: {
            int new_row = (row == 0) ? total_rows - 1 : row - 1;
            int cir = cells_in_row(new_row);
            col = (col < cir) ? col : cir - 1;
            row = new_row;
            break;
        }

        case KEY_DOWN: {
            int new_row = (row + 1) % total_rows;
            int cir = cells_in_row(new_row);
            col = (col < cir) ? col : cir - 1;
            row = new_row;
            break;
        }

        default:
            return;
    }

    g_selected = row * MEM_CELLS_PER_ROW + col;
}
