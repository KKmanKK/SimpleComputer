#include "mySimpleComputer.h"

int sc_commandEncode(int sign, int command, int operand, int *value) {
    if (value == NULL)
        return -1;
    if (sign < 0 || sign > 1)
        return -1;
    if (command < 0 || command > 255)
        return -1;
    if (operand < 0 || operand > 255)
        return -1;
    *value = (sign << 16) | (command << 8) | operand;
    return 0;
}
