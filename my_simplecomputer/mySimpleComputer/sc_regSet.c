#include "mySimpleComputer.h"

int sc_regSet(int flag, int value) {
    if (flag < 1 || flag > FLAG_COUNT)
        return -1;
    if (value)
        sc_flags = sc_flags | (1 << (flag - 1));
    else
        sc_flags = sc_flags & ~(1 << (flag - 1));
    return 0;
}
