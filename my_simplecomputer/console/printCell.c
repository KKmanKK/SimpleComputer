#include "console.h"

void printCell(int address, enum colors fg, enum colors bg) {
    int value;
    if (sc_memoryGet(address, &value) != 0)
        return;

    int row = MEM_ROW_ORIGIN + (address / MEM_CELLS_PER_ROW);
    int col = MEM_COL_ORIGIN + (address % MEM_CELLS_PER_ROW) * MEM_CELL_WIDTH;

    mt_gotoXY(row, col);
    mt_setfgcolor(fg);
    mt_setbgcolor(bg);

    int sign, command, operand;
    sc_commandDecode(value, &sign, &command, &operand);

    /* Each cell is MEM_CELL_WIDTH chars wide: sign + 4 hex + space */
    if (sign == 1)
        printf("+%04X ", value & 0xFFFF);
    else
        printf(" %04X ", value & 0xFFFF);

    mt_setdefaultcolor();
}
