#include "mySimpleComputer.h"

int sc_commandDecode(int value, int* command, int* operand) {
    if (value < 0 || value > 131071) {
        registr = registr | (1 << (COMMAND_NOT_FOUND - 1));
        return -1;
    }
    else {

        *command = (value >> 8) & 0xFF;
        *operand = value & 0xFF;   

        return 0;
    }
}