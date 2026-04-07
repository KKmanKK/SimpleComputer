#include "mySimpleComputer.h"

int sc_commandEncode(int command, int operand, int* value) {
    if (command < 0 || command > 127 || operand < 0 || operand > 127)
        return -1;
    else {
        *value = (command << 7) | operand;
        return 0;
    }
}