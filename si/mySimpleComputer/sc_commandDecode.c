#include "mySimpleComputer.h"

/* Max encoded value: (1<<16)|(255<<8)|255 = 131071 */
#define MAX_ENCODED 131071

int sc_commandDecode(int value, int *sign, int *command, int *operand) {
    if (value < 0 || value > MAX_ENCODED)
        return -1;
    if (sign == NULL || command == NULL || operand == NULL)
        return -1;
    *sign    = (value >> 16) & 0x1;
    *command = (value >>  8) & 0xFF;
    *operand =  value        & 0xFF;
    return 0;
}
