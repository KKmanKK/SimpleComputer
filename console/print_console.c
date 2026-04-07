#include "console.h"

int print_console(int* rows, int* cols) {

        mt_clrscr();    
        draw_console(rows, cols);        
        print_memory(sc_memory, selected_cell_index);
        print_accumulator(accumulator);
        print_instruction_counter(selected_cell_index);
        print_operation();
        print_flags();
        fflush(stdout);

        return 0;
}