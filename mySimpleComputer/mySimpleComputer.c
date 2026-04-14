#include "mySimpleComputer.h"

/* Global variables */
int sc_memory[MEMORY_SIZE];
int sc_flags;
int sc_accumulator;
int sc_icounter;

/* Valid commands list */
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

/* Memory functions */
int sc_memoryInit(void) {
    for (int i = 0; i < MEMORY_SIZE; i++)
        sc_memory[i] = 0;
    return 0;
}

int sc_memorySet(int address, int value) {
    if (address < 0 || address >= MEMORY_SIZE)
        return -1;
    if (value < MEMORY_MIN_VALUE || value > MEMORY_MAX_VALUE)
        return -1;
    sc_memory[address] = value;
    return 0;
}

int sc_memoryGet(int address, int *value) {
    if (address < 0 || address >= MEMORY_SIZE)
        return -1;
    if (value == NULL)
        return -1;
    *value = sc_memory[address];
    return 0;
}

int sc_memorySave(char *filename) {
    if (filename == NULL)
        return -1;
    FILE *file = fopen(filename, "wb");
    if (file == NULL)
        return -1;
    size_t written = fwrite(sc_memory, sizeof(int), MEMORY_SIZE, file);
    fclose(file);
    return (written == MEMORY_SIZE) ? 0 : -1;
}

int sc_memoryLoad(char *filename) {
    if (filename == NULL)
        return -1;
    FILE *file = fopen(filename, "rb");
    if (file == NULL)
        return -1;
    int tmp[MEMORY_SIZE];
    size_t read = fread(tmp, sizeof(int), MEMORY_SIZE, file);
    fclose(file);
    if (read != MEMORY_SIZE)
        return -1;
    for (int i = 0; i < MEMORY_SIZE; i++)
        sc_memory[i] = tmp[i];
    return 0;
}

/* Flags register functions */
int sc_regInit(void) {
    sc_flags = 0;
    return 0;
}

int sc_regSet(int flag, int value) {
    if (flag < 1 || flag > FLAG_COUNT)
        return -1;
    if (value)
        sc_flags = sc_flags | (1 << (flag - 1));
    else
        sc_flags = sc_flags & ~(1 << (flag - 1));
    return 0;
}

int sc_regGet(int flag, int *value) {
    if (flag < 1 || flag > FLAG_COUNT)
        return -1;
    if (value == NULL)
        return -1;
    *value = (sc_flags >> (flag - 1)) & 0x1;
    return 0;
}

/* Accumulator functions */
int sc_accumulatorInit(void) {
    sc_accumulator = 0;
    return 0;
}

int sc_accumulatorSet(int value) {
    if (value < ACC_MIN_VALUE || value > ACC_MAX_VALUE)
        return -1;
    sc_accumulator = value;
    return 0;
}

int sc_accumulatorGet(int *value) {
    if (value == NULL)
        return -1;
    *value = sc_accumulator;
    return 0;
}

/* Instruction counter functions */
int sc_icounterInit(void) {
    sc_icounter = 0;
    return 0;
}

int sc_icounterSet(int value) {
    if (value < 0 || value >= MEMORY_SIZE)
        return -1;
    sc_icounter = value;
    return 0;
}

int sc_icounterGet(int *value) {
    if (value == NULL)
        return -1;
    *value = sc_icounter;
    return 0;
}

/* Command functions */
#define MAX_ENCODED 131071

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

int sc_commandValidate(int command) {
    int n = sizeof(valid_commands) / sizeof(valid_commands[0]);
    for (int i = 0; i < n; i++) {
        if (valid_commands[i] == command)
            return 0;
    }
    return -1;
}