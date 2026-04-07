#include "mySimpleComputer.h"

int sc_regSet(int registrNum, int value) {
    if (registrNum < 0 || registrNum > REGISTER_SIZE) {
        return -1;
    }
    else {
        if (value == 1)
            registr = registr | (1 << (registrNum - 1));
        else {
            if (value == 0)
                registr = registr & (~(1 << (registrNum - 1)));
            else return -1;
        }         
        return 0;
    }
}