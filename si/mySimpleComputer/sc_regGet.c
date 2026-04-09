#include "mySimpleComputer.h"

int sc_regGet(int flag, int *value) {
    if (flag < 1 || flag > FLAG_COUNT)
        return -1;
    if (value == NULL)
        return -1;
    *value = (sc_flags >> (flag - 1)) & 0x1;
    return 0;
}
