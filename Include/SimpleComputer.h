#ifndef __GUARD_MYSIMPLECOMPUTER_H
#define __GUARD_MYSIMPLECOMPUTER_H

#include <stdio.h>
#include <stdlib.h>

#define MEMORY_SIZE 128

/* Valid range for memory cells (covers both data 0..65535 and commands 0..131071) */
#define MEMORY_MIN_VALUE  0
#define MEMORY_MAX_VALUE  131071

/* Valid range for accumulator (arithmetic data) */
#define ACC_MIN_VALUE  (-9999)
#define ACC_MAX_VALUE  9999

/* Flag register bit positions (1-based) */
#define FLAG_MEMORY_OUT_OF_BOUNDS   1
#define FLAG_COMMAND_NOT_FOUND      2
#define FLAG_DIVISION_BY_ZERO       3
#define FLAG_OVERFLOW               4
#define FLAG_CLOCK_PULSES_IGNORED   5
#define FLAG_COUNT                  5

extern int sc_memory[MEMORY_SIZE];
extern int sc_flags;
extern int sc_accumulator;
extern int sc_icounter;

/* Memory */
int sc_memoryInit(void);
int sc_memorySet(int address, int value);
int sc_memoryGet(int address, int *value);
int sc_memorySave(char *filename);
int sc_memoryLoad(char *filename);

/* Flags register */
int sc_regInit(void);
int sc_regSet(int flag, int value);
int sc_regGet(int flag, int *value);

/* Accumulator */
int sc_accumulatorInit(void);
int sc_accumulatorSet(int value);
int sc_accumulatorGet(int *value);

/* Instruction counter */
int sc_icounterInit(void);
int sc_icounterSet(int value);
int sc_icounterGet(int *value);

/* Command encode/decode/validate */
int sc_commandEncode(int sign, int command, int operand, int *value);
int sc_commandDecode(int value, int *sign, int *command, int *operand);
int sc_commandValidate(int command);

#endif /* __GUARD_MYSIMPLECOMPUTER_H */