#include "console.h"

int print_instruction_counter(int selected_cell_index) {
    if (selected_cell_index < 0 || selected_cell_index > 99) return -1;

    //mt_gotoXY (75, 5); printf("+%04x", selected_cell_index);
    mt_gotoXY (77, 5); printf("%d", selected_cell_index);
    mt_gotoXY (0, 31);
    return 0;
}