//
// Created by kchipson on 23.04.2020.
//

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <signal.h>
#include "SimpleComputer.h"

short int sc_memory[SC_MEM_SIZE];
uint8_t sc_regFLAGS;

short int sc_accumulator;
uint8_t sc_instructionCounter;
bool sc_running = false;
bool sc_stepMode = false;

/// Инициализирует оперативную память SC, задавая всем её ячейкам нулевые значения
/// \return 0
int sc_memoryInit(void) {
    sc_instructionCounter = 0;
    for (int i = 0; i < SC_MEM_SIZE; ++i)
        sc_memory[i] = 0;
    return 0;
}

/// Задает значение указанной ячейки памяти
/// \param address - ячейка памяти
/// \param value - значение
/// \return 0 - в случае успешного выполнения, -1 - в случае ошибки
int sc_memorySet(int8_t address, short int value) {
    if (address < 0 || address >= SC_MEM_SIZE) {
        sc_regSet(OUT_OF_MEMORY, true);
        return -1;
    }
    
    sc_memory[address] = value;
    return 0;
}

/// Возвращает значение указанной ячейки памяти
/// \param address - ячейка памяти
/// \param value - значение
/// \return 0 - в случае успешного выполнения, -1 - в случае ошибки
int sc_memoryGet(int8_t address, short int *value) {
    if (address < 0 || address >= SC_MEM_SIZE) {
        sc_regSet(OUT_OF_MEMORY, true);
        return -1;
    }
    *value = sc_memory[address];
    return 0;
}

/// Сохраняет содержимое памяти в файл в бинарном виде
/// \param filename - имя файла
/// \return 0 - в случае успешного выполнения, -1 - в случае ошибки
int sc_memorySave(char* filename) {
    FILE *fb;
    if (!(fb = fopen(filename, "wb"))) {
        return -1;
    }
    fwrite(sc_memory, sizeof(sc_memory), 1, fb);
    fclose(fb);
    return 0;
}

/// Загружает из указанного файла содержимое оперативной памяти
/// \param filename - имя файла
/// \return 0 - в случае успешного выполнения, -1 - в случае ошибки
int sc_memoryLoad(char* filename) {
    FILE *fb;
    if (!(fb = fopen(filename, "rb"))) {
        return -1;
    }
    fread(sc_memory, sizeof(sc_memory), 1, fb);
    fclose(fb);
    return 0;
}

/// Инициализирует регистр флагов нулевым значением
/// \return 0
int sc_regInit(void) {
    sc_regFLAGS = 0;
    return 0;
}

/// Устанавливает значение указанного регистра флагов
/// \param reg - флаг
/// \param value - значение
/// \return 0 - в случае успешного выполнения, -1 - в случае ошибки
int sc_regSet(int8_t reg, bool value) {
    if (reg < 0 || reg >= SC_REG_SIZE)
        return -1;
    
    if (value)
        sc_regFLAGS |= (1 << reg);
    else
        sc_regFLAGS &= ~(1 << reg);
    
    return 0;
}

/// Возвращает значение указанного флага
/// \param reg - флаг
/// \param value - значение
/// \return 0 - в случае успешного выполнения, -1 - в случае ошибки
int sc_regGet(int8_t reg, bool *value) {
    if (reg < 0 || reg >= SC_REG_SIZE)
        return -1;
    
    *value = (sc_regFLAGS & (1 << reg)) != 0;
    return 0;
}

/// Кодирует команду с указанным номером и операндом и помещает результат в value
/// \param command - команда
/// \param operand - операнд
/// \param value - значение
/// \return 0 - в случае успешного выполнения, -1 - в случае ошибки
int sc_commandEncode(short int command, short int operand, short int *value) {
    if (!(command > 0x9 && command < 0x12) && 
        !(command > 0x19 && command < 0x22) && 
        !(command > 0x29 && command < 0x34) && 
        !(command > 0x39 && command < 0x77))
        return -1;
    
    if ((operand < 0) || (operand > 0x7F))
        return -1;
    
    *value = 0;
    /* Операнд */
    for (int i = 0; i < 7; i++) {
        int8_t bit = (operand >> i) & 1;
        *value |= (bit << i);
    }
    /* Команда */
    for (int i = 0; i < 7; i++) {
        int8_t bit = (command >> i) & 1;
        *value |= (bit << (i + 7));
    }
    return 0;
}

/// Декодирует значение как команду SC
/// \param value - значение
/// \param command - команда
/// \param operand - операнд
/// \return 0 - в случае успешного выполнения, -1 - в случае ошибки
int sc_commandDecode(short int value, short int *command, short int *operand) {
    int tmpCom = 0, tmpOp = 0;
    
    if ((value >> 14) & 1) {
        sc_regSet(INCORRECT_COMMAND, true);
        return -1;
    }
    
    for (int i = 0; i < 7; i++) {
        int bit = (value >> i) & 1;
        tmpOp |= (bit << i);
    }
    
    for (int i = 0; i < 7; i++) {
        int bit = (value >> (i + 7)) & 1;
        tmpCom |= (bit << i);
    }
    
    if (!(tmpCom > 0x9 && tmpCom < 0x12) && 
        !(tmpCom > 0x19 && tmpCom < 0x22) && 
        !(tmpCom > 0x29 && tmpCom < 0x34) && 
        !(tmpCom > 0x39 && tmpCom < 0x77)) {
        sc_regSet(INCORRECT_COMMAND, true);
        return -1;
    }
    
    if ((tmpOp < 0) || (tmpOp > 0x7F)) {
        sc_regSet(INCORRECT_COMMAND, true);
        return -1;
    }
    
    *command = tmpCom;
    *operand = tmpOp;
    return 0;
}

/// Функция для получения текущей команды
int sc_getCurrentCommand(short int *command, short int *operand) {
    short int value;
    if (sc_memoryGet(sc_instructionCounter, &value) != 0) {
        return -1;
    }
    
    if (((value >> 14) & 1) == 0) {
        sc_regSet(INCORRECT_COMMAND, true);
        return -1;
    }
    
    return sc_commandDecode(value, command, operand);
}

// Добавьте в начало файла после других include
#include <signal.h>
#include <unistd.h>

// ... остальной код ...

// IRC - контроллер прерываний
void IRC(int signum) {
    bool ignoreTact;
    sc_regGet(IGNORING_TACT_PULSES, &ignoreTact);
    
    if (signum == SIGALRM) {
        if (!ignoreTact && sc_running) {
            CU();
            // Перезапускаем таймер только если модель все еще работает
            if (sc_running) {
                alarm(1);
            }
        }
    } else if (signum == SIGUSR1) {
        // Сброс системы
        sc_regInit();
        sc_instructionCounter = 0;
        sc_accumulator = 0;
        sc_running = false;
        sc_stepMode = false;
        sc_regSet(IGNORING_TACT_PULSES, true);
        alarm(0);  // Остановка таймера
    }
}

// Запуск модели
void sc_start(void) {
    if (!sc_running) {
        sc_running = true;
        sc_stepMode = false;
        sc_regSet(IGNORING_TACT_PULSES, false);
        
        // Сброс флагов ошибок перед запуском
        sc_regSet(OVERFLOW, false);
        sc_regSet(DIVISION_ERR_BY_ZERO, false);
        sc_regSet(OUT_OF_MEMORY, false);
        sc_regSet(INCORRECT_COMMAND, false);
        
        alarm(1);  // Запуск таймера для тактов
    }
}

// Остановка модели
void sc_stop(void) {
    sc_running = false;
    alarm(0);
}

// Один шаг модели
void sc_step(void) {
    if (!sc_running) {
        sc_running = true;
        sc_stepMode = true;
        CU();
        sc_running = false;
        sc_stepMode = false;
        ui_update();  // Обновляем интерфейс после шага
    }
}

// Обновленная функция CU с проверкой на останов
void CU(void) {
    // Проверка флага игнорирования тактов
    bool ignoreTact;
    sc_regGet(IGNORING_TACT_PULSES, &ignoreTact);
    if (ignoreTact) {
        return;
    }
    
    // Проверка флага ошибки
    bool errorFlag;
    sc_regGet(INCORRECT_COMMAND, &errorFlag);
    if (errorFlag) {
        sc_running = false;
        alarm(0);
        return;
    }
    
    // Проверка, не вышли ли за пределы памяти
    if (sc_instructionCounter >= SC_MEM_SIZE) {
        sc_instructionCounter = 0;
        sc_regSet(OUT_OF_MEMORY, true);
        sc_running = false;
        alarm(0);
        return;
    }
    
    // Получение текущей команды
    short int command, operand;
    if (sc_getCurrentCommand(&command, &operand) != 0) {
        if (!sc_stepMode) {
            sc_instructionCounter++;
        }
        return;
    }
    
    // Выполнение команды
    int result = ALU(command, operand);
    
    // Увеличение счетчика команд (если не было перехода)
    bool incorrect;
    sc_regGet(INCORRECT_COMMAND, &incorrect);
    if (!incorrect && result == 0 && sc_running) {
        if (!sc_stepMode) {
            sc_instructionCounter++;
        }
    }
    
    // Обновляем интерфейс после каждого такта в пошаговом режиме
    if (sc_stepMode) {
        ui_update();
    }
}