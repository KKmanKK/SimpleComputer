#include "mySimpleComputer.h"

static const int valid_commands[] = {
    10, 11,                          /* READ, WRITE */
    20, 21,                          /* LOAD, STORE */
    30, 31, 32, 33,                  /* ADD, SUB, DIVIDE, MUL */
    40, 41, 42, 43,                  /* JUMP, JNEG, JZ, HALT */
    51, 52, 53, 54, 55, 56, 57, 58, 59, /* NOT, AND, OR, XOR, JNS, JC, JNC, JP, JNP */
    60, 61, 62, 63, 64, 65, 66,      /* CHL, SHR, RCL, RCR, NEG, ADDC, SUBC */
    67, 68, 69, 70,                  /* LOGLC, LOGRC, RCCL, RCCR */
    71, 72, 73, 74, 75, 76,          /* MOVA, MOVR, MOVCA, MOVCR, ADDC, SUBC */
    80, 81                           /* User: SQR, ABS */
};

int sc_commandValidate(int command) {
    int n = sizeof(valid_commands) / sizeof(valid_commands[0]);
    for (int i = 0; i < n; i++) {
        if (valid_commands[i] == command)
            return 0;
    }
    return -1;
}
