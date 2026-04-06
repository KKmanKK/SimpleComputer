#ifndef MYSIMPLECOMPUTER_H
#define MYSIMPLECOMPUTER_H

#include <time.h>

#define MEMORY_SIZE 100
#define REGISTER_SIZE 5

/* Регистры флагов */
#define REG_MEMORY_ERROR     1
#define REG_INVALID_COMMAND  2
#define REG_DIVISION_ZERO    3
#define REG_OVERFLOW         4
#define REG_IGNORE_TICKS     5

/* Глобальные переменные */
extern int sc_memory[MEMORY_SIZE];
extern int sc_accumulator;
extern int sc_icounter;
extern int sc_flags;
extern int cpu_cache[50];
extern int cpu_cache_lines[5];
extern time_t cache_access_time[5];

/* Функции памяти */
int sc_memoryInit(void);
int sc_memorySet(int address, int value);
int sc_memoryGet(int address, int *value);
int sc_memorySave(char *filename);
int sc_memoryLoad(char *filename);

/* Функции регистров */
int sc_regInit(void);
int sc_regSet(int reg, int value);
int sc_regGet(int reg, int *value);
int sc_accumulatorInit(void);
int sc_accumulatorSet(int value);
int sc_accumulatorGet(int *value);
int sc_icounterInit(void);
int sc_icounterSet(int value);
int sc_icounterGet(int *value);

/* Функции команд */
int sc_commandEncode(int command, int operand, int *value);
int sc_commandDecode(int value, int *command, int *operand);

/* Функции кэша */
int sc_cacheInit(void);

#endif