#ifndef __GUARD_MYSIMPLECOMPUTER_H
#define __GUARD_MYSIMPLECOMPUTER_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define MEMORY_SIZE 100
#define REGISTER_SIZE 5

#define MEMORY_OUT_OF_BOUNDS 1         // 00001
#define COMMAND_NOT_FOUND 2            // 00010
#define DIVISION_BY_ZERO_ERROR 3       // 00100
#define OVERFLOW_DURING_OPERATION 4    // 01000
#define CLOCK_PULSES_IGNORED 5         // 10000

extern int sc_memory[MEMORY_SIZE];
extern int CPU_cache[50];
extern int CPU_cache_lines[5];
extern time_t access_cache_time[5];
extern int registr;

int sc_memoryInit();

int sc_memorySet(int address, int value);

int sc_memoryGet(int address, int* value);

int sc_memorySave(char* filename);

int sc_memoryLoad(char* filename);

int sc_regInit(void);

int sc_regSet(int registrNum, int value);

int sc_regGet(int registrNum, int* value);

int sc_commandEncode(int command, int operand, int* value);

int sc_commandDecode(int value, int* command, int* operand);

int sc_cpuCacheInit(void);

#endif /* __GUARD_MYSIMPLECOMPUTER_H */