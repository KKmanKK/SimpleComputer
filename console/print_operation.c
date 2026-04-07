#include "console.h"

int print_operation() {

    if (((sc_memory[selected_cell_index] >> 16) & 1) == 1) {
            int command;
            int operand;

            sc_commandDecode(sc_memory[selected_cell_index], &command, &operand);

            mt_gotoXY (73, 8); printf("+%d : %d", command, operand);
            mt_gotoXY (0, 31);
    }
    return 0;
}