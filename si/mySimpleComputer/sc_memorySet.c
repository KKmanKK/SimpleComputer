#include "mySimpleComputer.h"

int sc_memorySet(int address, int value) {
    if (address < 0 || address >= MEMORY_SIZE)
        return -1;
    if (value < MEMORY_MIN_VALUE || value > MEMORY_MAX_VALUE)
        return -1;
    sc_memory[address] = value;
    return 0;
}
