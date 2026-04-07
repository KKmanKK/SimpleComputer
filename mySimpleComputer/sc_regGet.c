#include "mySimpleComputer.h"

int sc_regGet(int registrNum, int* value) {
    if (registrNum < 0 || registrNum > REGISTER_SIZE) {
        return -1;
    }
    else {
        *value = (registr >> (registrNum - 1)) & 0x1;
        return 0;
    }
}