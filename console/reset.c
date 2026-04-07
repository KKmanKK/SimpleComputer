#include "console.h"

int reset() {

    sc_memoryInit();
    accumulator = 0;
    instruction_counter = 0;
    selected_cell_index = 0;

    sc_regSet(1, 0); // MEMORY_OUT_OF_BOUNDS
    sc_regSet(2, 0); // COMMAND_NOT_FOUND
    sc_regSet(3, 0); // DIVISION_BY_ZERO_ERROR
    sc_regSet(4, 0); // OVERFLOW_DURING_OPERATION
    sc_regSet(5, 1); // CLOCK_PULSES_IGNORED

    for (int i = 0; i < 5; i++) {
        CPU_cache_lines[i] = -1;
    }

    sc_cpuCacheInit();

    return 0;
}