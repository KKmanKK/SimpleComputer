#include "console.h"

void select_next_cell(void) {
    if (selected_cell_index + 1 < MEMORY_SIZE)
        selected_cell_index += 1;
    else 
        selected_cell_index = 0;   
}