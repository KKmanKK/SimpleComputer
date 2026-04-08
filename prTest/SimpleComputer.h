//
// Created by kchipson on 23.04.2020.
//

#ifndef SIMPLECOMPUTER_H
#define SIMPLECOMPUTER_H

#include <stdbool.h>
#include <stdint.h>

// Регистры флагов (в соответствии с заданием)
#define FLAG_OVERFLOW 0              // P - Переполнение при выполнении операции
#define FLAG_DIVISION_BY_ZERO 1      // 0 - Ошибка деления на 0
#define FLAG_OUT_OF_MEMORY 2         // M - Ошибка выхода за границы памяти
#define FLAG_IGNORE_CLOCK 3          // T - Игнорирование тактовых импульсов
#define FLAG_INVALID_COMMAND 4       // E - Получена неверная команда

#define SC_REG_SIZE 5
#define SC_MEM_SIZE 128              // 128 ячеек (адреса 0-127)

// Коды операций (в соответствии с таблицей из задания)
// Операции ввода/вывода
#define CMD_NOP      0x00
#define CMD_CPUINFO  0x01
#define CMD_READ     0x0A
#define CMD_WRITE    0x0B

// Операции загрузки/выгрузки
#define CMD_LOAD     0x14
#define CMD_STORE    0x15

// Арифметические операции
#define CMD_ADD      0x1E
#define CMD_SUB      0x1F
#define CMD_DIVIDE   0x20
#define CMD_MUL      0x21

// Операции передачи управления
#define CMD_JUMP     0x28
#define CMD_JNEG     0x29
#define CMD_JZ       0x2A
#define CMD_HALT     0x2B

// Пользовательские функции
#define CMD_NOT      0x33
#define CMD_AND      0x34
#define CMD_OR       0x35
#define CMD_XOR      0x36
#define CMD_JNS      0x37
#define CMD_JC       0x38
#define CMD_JNC      0x39
#define CMD_JP       0x3A
#define CMD_JNP      0x3B
#define CMD_CHL      0x3C
#define CMD_SHR      0x3D
#define CMD_RCL      0x3E
#define CMD_RCR      0x3F
#define CMD_NEG      0x40
#define CMD_ADDC     0x41
#define CMD_SUBC     0x42
#define CMD_LOGLC    0x43
#define CMD_LOGRC    0x44
#define CMD_RCCL     0x45
#define CMD_RCCR     0x46
#define CMD_MOVA     0x47
#define CMD_MOVR     0x48
#define CMD_MOVCA    0x49
#define CMD_MOVCR    0x4A
#define CMD_ADDC_M   0x4B
#define CMD_SUBC_M   0x4C

extern short int sc_memory[SC_MEM_SIZE];
extern uint8_t sc_regFLAGS;
extern short int sc_accumulator;
extern uint8_t sc_instructionCounter;
extern bool sc_running;
extern bool sc_stepMode;

int sc_memoryInit(void);
int sc_memorySet(int address, short int value);
int sc_memoryGet(int address, short int *value);
int sc_memorySave(char *filename);
int sc_memoryLoad(char *filename);
int sc_regInit(void);
int sc_regSet(int reg, int value);
int sc_regGet(int reg, int *value);
int sc_accumulatorInit(void);
int sc_accumulatorSet(int value);
int sc_accumulatorGet(int *value);
int sc_icounterInit(void);
int sc_icounterSet(int value);
int sc_icounterGet(int *value);
int sc_commandEncode(int sign, int command, int operand, int *value);
int sc_commandDecode(int value, int *sign, int *command, int *operand);
int sc_commandValidate(int command);

int ALU(int command, int operand);
void CU(void);
void IRC(int signum);
void sc_start(void);
void sc_stop(void);
void sc_step(void);
int sc_getCurrentCommand(int *command, int *operand);

#endif // SIMPLECOMPUTER_H