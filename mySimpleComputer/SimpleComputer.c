//
// Created by kchipson on 23.04.2020.
//

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <signal.h>
#include "SimpleComputer.h"
#include <stdlib.h>   // Добавить для strtol()
#include <string.h>   // Добавить для strlen() если нужно

short int sc_memory[SC_MEM_SIZE];
uint8_t sc_regFLAGS;
short int sc_accumulator;
uint8_t sc_instructionCounter;
bool sc_running = false;
bool sc_stepMode = false;

/// Инициализирует оперативную память SC, задавая всем её ячейкам нулевые значения
int sc_memoryInit(void) {
    for (int i = 0; i < SC_MEM_SIZE; ++i)
        sc_memory[i] = 0;
    return 0;
}

/// Задает значение указанной ячейки памяти
int sc_memorySet(int address, short int value) {
    if (address < 0 || address >= SC_MEM_SIZE) {
        sc_regSet(FLAG_OUT_OF_MEMORY, 1);
        return -1;
    }
    sc_memory[address] = value;
    return 0;
}

/// Возвращает значение указанной ячейки памяти
int sc_memoryGet(int address, short int *value) {
    if (address < 0 || address >= SC_MEM_SIZE) {
        sc_regSet(FLAG_OUT_OF_MEMORY, 1);
        return -1;
    }
    *value = sc_memory[address];
    return 0;
}

/// Сохраняет содержимое памяти в файл в бинарном виде
int sc_memorySave(char *filename) {
    FILE *fb = fopen(filename, "wb");
    if (!fb) return -1;
    fwrite(sc_memory, sizeof(short int), SC_MEM_SIZE, fb);
    fclose(fb);
    return 0;
}

/// Загружает из указанного файла содержимое оперативной памяти
int sc_memoryLoad(char *filename) {
    FILE *fb = fopen(filename, "rb");
    if (!fb) return -1;
    fread(sc_memory, sizeof(short int), SC_MEM_SIZE, fb);
    fclose(fb);
    return 0;
}

/// Инициализирует регистр флагов значениями по умолчанию
int sc_regInit(void) {
    sc_regFLAGS = 0;
    sc_regSet(FLAG_IGNORE_CLOCK, 1);  // T = 1 по умолчанию
    return 0;
}

/// Устанавливает значение указанного регистра флагов
int sc_regSet(int reg, int value) {
    if (reg < 0 || reg >= SC_REG_SIZE) return -1;
    if (value)
        sc_regFLAGS |= (1 << reg);
    else
        sc_regFLAGS &= ~(1 << reg);
    return 0;
}

/// Возвращает значение указанного флага
int sc_regGet(int reg, int *value) {
    if (reg < 0 || reg >= SC_REG_SIZE) return -1;
    *value = (sc_regFLAGS & (1 << reg)) ? 1 : 0;
    return 0;
}

/// Инициализирует аккумулятор значением по умолчанию
int sc_accumulatorInit(void) {
    sc_accumulator = 0;
    return 0;
}

/// Устанавливает значение аккумулятора
int sc_accumulatorSet(int value) {
    if (value < -32768 || value > 32767) return -1;
    sc_accumulator = (short int)value;
    return 0;
}

/// Возвращает значение аккумулятора
int sc_accumulatorGet(int *value) {
    if (!value) return -1;
    *value = sc_accumulator;
    return 0;
}

/// Инициализирует счетчик команд
int sc_icounterInit(void) {
    sc_instructionCounter = 0;
    return 0;
}

/// Устанавливает значение счетчика команд
int sc_icounterSet(int value) {
    if (value < 0 || value >= SC_MEM_SIZE) return -1;
    sc_instructionCounter = (uint8_t)value;
    return 0;
}

/// Возвращает значение счетчика команд
int sc_icounterGet(int *value) {
    if (!value) return -1;
    *value = sc_instructionCounter;
    return 0;
}

/// Кодирует команду
int sc_commandEncode(int sign, int command, int operand, int *value) {
    if (sign != 0 && sign != 1) return -1;
    if (sc_commandValidate(command) != 0) return -1;
    if (operand < 0 || operand > 0x7F) return -1;
    
    *value = 0;
    *value |= (sign << 14);
    *value |= ((command & 0x7F) << 7);
    *value |= (operand & 0x7F);
    return 0;
}

/// Декодирует значение как команду
int sc_commandDecode(int value, int *sign, int *command, int *operand) {
    *sign = (value >> 14) & 1;
    *command = (value >> 7) & 0x7F;
    *operand = value & 0x7F;
    
    if (sc_commandValidate(*command) != 0) {
        sc_regSet(FLAG_INVALID_COMMAND, 1);
        return -1;
    }
    return 0;
}

/// Проверяет корректность команды
int sc_commandValidate(int command) {
    // Проверка по всем допустимым командам
    switch(command) {
        case CMD_NOP: case CMD_CPUINFO:
        case CMD_READ: case CMD_WRITE:
        case CMD_LOAD: case CMD_STORE:
        case CMD_ADD: case CMD_SUB: case CMD_DIVIDE: case CMD_MUL:
        case CMD_JUMP: case CMD_JNEG: case CMD_JZ: case CMD_HALT:
        case CMD_NOT: case CMD_AND: case CMD_OR: case CMD_XOR:
        case CMD_JNS: case CMD_JC: case CMD_JNC: case CMD_JP: case CMD_JNP:
        case CMD_CHL: case CMD_SHR: case CMD_RCL: case CMD_RCR: case CMD_NEG:
        case CMD_ADDC: case CMD_SUBC: case CMD_LOGLC: case CMD_LOGRC:
        case CMD_RCCL: case CMD_RCCR: case CMD_MOVA: case CMD_MOVR:
        case CMD_MOVCA: case CMD_MOVCR: case CMD_ADDC_M: case CMD_SUBC_M:
            return 0;
        default:
            return -1;
    }
}

/// Получение текущей команды
int sc_getCurrentCommand(int *command, int *operand) {
    short int value;
    int sign;
    if (sc_memoryGet(sc_instructionCounter, &value) != 0) return -1;
    return sc_commandDecode(value, &sign, command, operand);
}

/// Арифметико-логическое устройство
int ALU(int command, int operand) {
    short int memory_value;
    int result;
    
    switch(command) {
        case CMD_ADD:
            if (sc_memoryGet(operand, &memory_value) != 0) return -1;
            result = sc_accumulator + memory_value;
            if (result > 32767 || result < -32768) {
                sc_regSet(FLAG_OVERFLOW, 1);
                return -1;
            }
            sc_accumulator = (short int)result;
            break;
            
        case CMD_SUB:
            if (sc_memoryGet(operand, &memory_value) != 0) return -1;
            result = sc_accumulator - memory_value;
            if (result > 32767 || result < -32768) {
                sc_regSet(FLAG_OVERFLOW, 1);
                return -1;
            }
            sc_accumulator = (short int)result;
            break;
            
        case CMD_DIVIDE:
            if (sc_memoryGet(operand, &memory_value) != 0) return -1;
            if (memory_value == 0) {
                sc_regSet(FLAG_DIVISION_BY_ZERO, 1);
                return -1;
            }
            sc_accumulator = (short int)(sc_accumulator / memory_value);
            break;
            
        case CMD_MUL:
            if (sc_memoryGet(operand, &memory_value) != 0) return -1;
            result = sc_accumulator * memory_value;
            if (result > 32767 || result < -32768) {
                sc_regSet(FLAG_OVERFLOW, 1);
                return -1;
            }
            sc_accumulator = (short int)result;
            break;
            
        case CMD_LOAD:
            if (sc_memoryGet(operand, &sc_accumulator) != 0) return -1;
            break;
            
        case CMD_STORE:
            if (sc_memorySet(operand, sc_accumulator) != 0) return -1;
            break;
            
        case CMD_JUMP:
            sc_instructionCounter = (uint8_t)operand - 1;
            break;
            
        case CMD_JNEG:
            if (sc_accumulator < 0)
                sc_instructionCounter = (uint8_t)operand - 1;
            break;
            
        case CMD_JZ:
            if (sc_accumulator == 0)
                sc_instructionCounter = (uint8_t)operand - 1;
            break;
            
        case CMD_HALT:
            sc_running = false;
            break;
            
        case CMD_NOT:
            if (sc_memorySet(operand, (short int)(~sc_accumulator)) != 0) return -1;
            break;
            
        case CMD_AND:
            if (sc_memoryGet(operand, &memory_value) != 0) return -1;
            sc_accumulator = sc_accumulator & memory_value;
            break;
            
        case CMD_OR:
            if (sc_memoryGet(operand, &memory_value) != 0) return -1;
            sc_accumulator = sc_accumulator | memory_value;
            break;
            
        case CMD_XOR:
            if (sc_memoryGet(operand, &memory_value) != 0) return -1;
            sc_accumulator = sc_accumulator ^ memory_value;
            break;
            
        case CMD_JNS:
            if (sc_accumulator > 0)
                sc_instructionCounter = (uint8_t)operand - 1;
            break;
        case CMD_READ:
            // READ - ввод с терминала
            if (sc_memoryGet(operand, &memory_value) != 0) return -1;
            printf("\n[READ] Enter value for address 0x%02X (hex): ", operand);
            fflush(stdout);

            char input[20];
            if (fgets(input, sizeof(input), stdin)) {
                int val;
                sscanf(input, "%x", &val);
                sc_memorySet(operand, (short int)val);
            }
        break;

        case CMD_WRITE:
            // WRITE - вывод на терминал
            if (sc_memoryGet(operand, &memory_value) != 0) return -1;
            printf("\n[WRITE] Address 0x%02X = 0x%04X (%d)\n", 
                operand, memory_value & 0xFFFF, memory_value);
            fflush(stdout);
        break;
                
    default:
        sc_regSet(FLAG_INVALID_COMMAND, 1);
        return -1;
}
    return 0;
}

/// Управляющее устройство
void CU(void) {
    int ignoreTact;
    sc_regGet(FLAG_IGNORE_CLOCK, &ignoreTact);
    if (ignoreTact) return;
    
    int errorFlag;
    sc_regGet(FLAG_INVALID_COMMAND, &errorFlag);
    if (errorFlag) {
        sc_running = false;
        alarm(0);
        return;
    }
    
    if (sc_instructionCounter >= SC_MEM_SIZE) {
        sc_instructionCounter = 0;
        sc_regSet(FLAG_OUT_OF_MEMORY, 1);
        sc_running = false;
        alarm(0);
        return;
    }
    
    int command, operand;
    if (sc_getCurrentCommand(&command, &operand) != 0) {
        if (!sc_stepMode) sc_instructionCounter++;
        return;
    }
    
    // Если команда HALT - останавливаем
    if (command == CMD_HALT) {
        sc_running = false;
        alarm(0);
        printf("\n[HALT] Program stopped\n");
        return;
    }
    
    int result = ALU(command, operand);
    
    int incorrect;
    sc_regGet(FLAG_INVALID_COMMAND, &incorrect);
    if (!incorrect && result == 0 && sc_running) {
        if (!sc_stepMode) sc_instructionCounter++;
    }
    
    if (sc_instructionCounter >= SC_MEM_SIZE) {
        sc_instructionCounter = 0;
        sc_regSet(FLAG_OUT_OF_MEMORY, 1);
        sc_running = false;
        alarm(0);
    }
}

void IRC(int signum) {
    int ignoreTact;
    sc_regGet(FLAG_IGNORE_CLOCK, &ignoreTact);
    
    if (signum == SIGALRM) {
        if (!ignoreTact && sc_running) {
            CU();
            
            int invalid_cmd;
            sc_regGet(FLAG_INVALID_COMMAND, &invalid_cmd);
            if (sc_running && !invalid_cmd) {
                alarm(1);
            }
        }
    } else if (signum == SIGUSR1) {
        alarm(0);
        sc_regInit();
        sc_instructionCounter = 0;
        sc_accumulator = 0;
        sc_running = false;
        sc_stepMode = false;
    } else if (signum == SIGINT) {
        // Убираем 'running' - этой переменной нет в SimpleComputer.c
        sc_running = false;
        alarm(0);
    }
}

/// Запуск модели
void sc_start(void) {
    if (!sc_running) {
        sc_running = true;
        sc_stepMode = false;
        
        sc_regSet(FLAG_OVERFLOW, 0);
        sc_regSet(FLAG_DIVISION_BY_ZERO, 0);
        sc_regSet(FLAG_OUT_OF_MEMORY, 0);
        sc_regSet(FLAG_INVALID_COMMAND, 0);
        sc_regSet(FLAG_IGNORE_CLOCK, 0);
        
        // Для отладки - автоматическая остановка через 3 секунды
        // Это позволит выйти из бесконечного цикла
        alarm(3);
    }
}

/// Остановка модели
void sc_stop(void) {
    sc_running = false;
    alarm(0);
}

/// Один шаг модели
void sc_step(void) {
    if (!sc_running) {
        // Сохраняем текущее состояние флага T
        int old_ignore;
        sc_regGet(FLAG_IGNORE_CLOCK, &old_ignore);
        
        sc_running = true;
        sc_stepMode = true;
        
        // Временно разрешаем выполнение (если было запрещено)
        sc_regSet(FLAG_IGNORE_CLOCK, 0);
        
        // Выполняем одну команду
        CU();
        
        // Восстанавливаем состояние флага T
        sc_regSet(FLAG_IGNORE_CLOCK, old_ignore);
        
        sc_running = false;
        sc_stepMode = false;
    }
}

