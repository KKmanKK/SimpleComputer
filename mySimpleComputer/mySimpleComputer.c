#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "../include/mySimpleComputer.h"

/* Глобальные переменные */
int sc_memory[MEMORY_SIZE] = {0};
int sc_accumulator = 0;
int sc_icounter = 0;
int sc_flags = 0;

/* Кэш процессора */
int cpu_cache[50] = {0};
int cpu_cache_lines[5] = {-1, -1, -1, -1, -1};
time_t cache_access_time[5] = {0};

/* ========== ПАМЯТЬ ========== */

int sc_memoryInit(void) {
    for (int i = 0; i < MEMORY_SIZE; i++)
        sc_memory[i] = 0;
    return 0;
}

int sc_memorySet(int address, int value) {
    if (address < 0 || address >= MEMORY_SIZE) {
        sc_regSet(REG_MEMORY_ERROR, 1);
        return -1;
    }
    if (value < 0 || value > 0xFFFF) {
        return -1;
    }
    sc_memory[address] = value;
    return 0;
}

int sc_memoryGet(int address, int *value) {
    if (address < 0 || address >= MEMORY_SIZE) {
        sc_regSet(REG_MEMORY_ERROR, 1);
        return -1;
    }
    if (value == NULL) return -1;
    *value = sc_memory[address];
    return 0;
}

int sc_memorySave(char *filename) {
    FILE *f = fopen(filename, "wb");
    if (f == NULL) return -1;
    fwrite(sc_memory, sizeof(int), MEMORY_SIZE, f);
    fclose(f);
    return 0;
}

int sc_memoryLoad(char *filename) {
    FILE *f = fopen(filename, "rb");
    if (f == NULL) return -1;
    fread(sc_memory, sizeof(int), MEMORY_SIZE, f);
    fclose(f);
    return 0;
}

/* ========== РЕГИСТРЫ ========== */

int sc_regInit(void) {
    sc_flags = 1 << (REG_IGNORE_TICKS - 1);
    return 0;
}

int sc_regSet(int reg, int value) {
    if (reg < 1 || reg > REGISTER_SIZE) return -1;
    if (value == 1)
        sc_flags |= (1 << (reg - 1));
    else if (value == 0)
        sc_flags &= ~(1 << (reg - 1));
    else
        return -1;
    return 0;
}

int sc_regGet(int reg, int *value) {
    if (reg < 1 || reg > REGISTER_SIZE) return -1;
    if (value == NULL) return -1;
    *value = (sc_flags >> (reg - 1)) & 1;
    return 0;
}

int sc_accumulatorInit(void) {
    sc_accumulator = 0;
    return 0;
}

int sc_accumulatorSet(int value) {
    if (value < 0 || value > 0xFFFF) return -1;
    sc_accumulator = value;
    return 0;
}

int sc_accumulatorGet(int *value) {
    if (value == NULL) return -1;
    *value = sc_accumulator;
    return 0;
}

int sc_icounterInit(void) {
    sc_icounter = 0;
    return 0;
}

int sc_icounterSet(int value) {
    if (value < 0 || value >= MEMORY_SIZE) return -1;
    sc_icounter = value;
    return 0;
}

int sc_icounterGet(int *value) {
    if (value == NULL) return -1;
    *value = sc_icounter;
    return 0;
}

/* ========== КОМАНДЫ ========== */

int sc_commandEncode(int command, int operand, int *value) {
    if (command < 0 || command > 0x7F) return -1;
    if (operand < 0 || operand > 0x7F) return -1;
    if (value == NULL) return -1;
    *value = (command << 7) | operand;
    return 0;
}

int sc_commandDecode(int value, int *command, int *operand) {
    if (command == NULL || operand == NULL) return -1;
    *command = (value >> 7) & 0x7F;
    *operand = value & 0x7F;
    return 0;
}

/* ========== КЭШ ========== */

int sc_cacheInit(void) {
    for (int i = 0; i < 5; i++) {
        cpu_cache_lines[i] = -1;
        cache_access_time[i] = 0;
    }
    for (int i = 0; i < 50; i++)
        cpu_cache[i] = 0;
    return 0;
}