#include "mySimpleComputer.h"

int sc_icounterSet(int value) {
    if (value < 0 || value >= MEMORY_SIZE)
        return -1;
    sc_icounter = value;
    return 0;
}
