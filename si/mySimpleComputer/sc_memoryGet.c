#include "mySimpleComputer.h"

int sc_memoryGet(int address, int *value) {
    if (address < 0 || address >= MEMORY_SIZE)
        return -1;
    if (value == NULL)
        return -1;
    *value = sc_memory[address];
    return 0;
}
