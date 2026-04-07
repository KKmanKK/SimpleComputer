#ifndef __GUARD_CONSOLE_H
#define __GUARD_CONSOLE_H

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <stdatomic.h>
#include "myReadkey.h"
#include "myBigChars.h"
#include "myTerm.h"
#include "mySimpleComputer.h"

extern int accumulator;
extern int instruction_counter;
extern int selected_cell_index;
extern pthread_mutex_t lock;

int print_accumulator(int accumulator);

int print_instruction_counter(int selected_cell_index);

int set_instruction_counter(int *selected_cell_index);

int set_accumulator(int *accumulator);

int print_operation();

int print_flags(void);

int draw_console(int *rows, int *cols);

int print_big_chars(int value);

int print_memory(int sc_memory[], int selected_cell_index);

int memory_cell_selection (int direction, int* selected_cell_index);

void* clock_pulse_generator(void* arg);

int reset();

void CU();

int ALU(int command, int operand);

int set_memory_cell(int address);

int print_console(int* rows, int* cols);

void IRC(int signum);

void select_next_cell(void);

#endif /* __GUARD_CONSOLE_H */