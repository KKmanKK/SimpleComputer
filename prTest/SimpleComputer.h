//
// Created by kchipson on 23.04.2020.
//

#ifndef SIMPLECOMPUTER_H
#define SIMPLECOMPUTER_H

#include <stdbool.h>
#include <stdint.h>

#define OVERFLOW 0              // Переполнение при выполнении операции
#define DIVISION_ERR_BY_ZERO 1  // Ошибка деления на 0
#define OUT_OF_MEMORY 2         // Ошибка выхода за границы памяти
#define IGNORING_TACT_PULSES 3  // Игнорирование тактовых импульсов
#define INCORRECT_COMMAND 4     // Указана неверная команда

#define SC_REG_SIZE 5
#define SC_MEM_SIZE 100

extern short int sc_accumulator;
extern uint8_t sc_instructionCounter;
extern bool sc_running;        // Флаг работы модели
extern bool sc_stepMode;       // Флаг пошагового режима

int sc_memoryInit(void);
int sc_memorySet(int8_t address, short int value);
int sc_memoryGet(int8_t address, short int *value);
int sc_memorySave(char* filename);
int sc_memoryLoad(char* filename);
int sc_regInit(void);
int sc_regSet(int8_t reg, bool value);
int sc_regGet(int8_t reg, bool *value);
int sc_commandEncode(short int command, short int operand, short int *value);
int sc_commandDecode(short int value, short int *command, short int *operand);

// Новые функции для моделирования
int ALU(int command, int operand);
void CU(void);
void IRC(int signum);
void sc_start(void);
void sc_stop(void);
void sc_step(void);
int sc_getCurrentCommand(short int *command, short int *operand);

#endif // SIMPLECOMPUTER_H