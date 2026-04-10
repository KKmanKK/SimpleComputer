#include "mySimpleComputer.h"

int sc_accumulatorSet(int value) {
    if (value < ACC_MIN_VALUE || value > ACC_MAX_VALUE)
        return -1;
    sc_accumulator = value;
    return 0;
}
