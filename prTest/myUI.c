//
// Created by kchipson on 19.05.2020.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include "myUI.h"
#include "myReadkey.h"

int8_t currMemCell = 0;
bool editMode = false;
enum editTarget { TARGET_MEMORY, TARGET_ACCUMULATOR, TARGET_COUNTER } currentEditTarget = TARGET_MEMORY;

int drawingBoxes(void);
int drawingTexts(void);
int drawingMemory(void);
int drawingAccumulator(void);
int drawingInstructionCounter(void);
int drawingOperation(void);
int drawingFlags(void);
int drawingBigChar(void);
bool checkCorrectInput(const char buffer[10]);
int ui_messageOutput(char *str, enum colors color);
int clearBuffIn(void);
int editValueInPlace(int x, int y, short int *value, bool isCommand, int maxValue);
int getMemoryDisplayValue(short int value, char *output);
int setMemoryFromDisplayValue(const char *input, short int *value);

/// "Инициализация" интерфейса пользователя
/// \return 0 - в случае успешного выполнения, -1 - в случае ошибки
int ui_initial(void) {
    currMemCell = 0;
    editMode = false;
    if (rk_myTermSave())
        return -1;
    sc_memoryInit();
    sc_regInit();
    sc_regSet(IGNORING_TACT_PULSES, true);
    return 0;
}

/// Обновление интерфейса пользователя
/// \return 0 - в случае успешного выполнения, -1 - в случае ошибки
int ui_update(void) {
    if (!editMode) {
        mt_clrScreen();
        if (drawingBoxes())
            return -1;
        if (drawingTexts())
            return -1;
        if (drawingMemory())
            return -1;
        if (drawingAccumulator())
            return -1;
        if (drawingInstructionCounter())
            return -1;
        if (drawingOperation())
            return -1;
        if (drawingFlags())
            return -1;
        if (drawingBigChar())
            return -1;
        mt_gotoXY(1, 23);
        printf("Input/Output:\n");
    }
    return 0;
}

/// Перемещение указателя на выделенную ячейку
/// \param key - Клавиша
/// \return 0 - в случае успешного выполнения, -1 - в случае ошибки
int ui_moveCurrMemPointer(enum keys key) {
    if (editMode) return 0;
    
    switch (key) {
        case KEY_UP:    (currMemCell <= 9) ? (currMemCell = 90 + currMemCell) : (currMemCell -= 10); break;
        case KEY_RIGHT: (!((currMemCell + 1) % 10)) ? (currMemCell -= 9) : (currMemCell += 1); break;
        case KEY_DOWN:  (currMemCell >= 90) ? (currMemCell = currMemCell - 90) : (currMemCell += 10); break;
        case KEY_LEFT:  (!(currMemCell % 10)) ? (currMemCell += 9) : (currMemCell -= 1); break;
        default: return -1;
    }
    return 0;
}

/// Редактирование ячейки памяти InPlace
int editMemoryInPlace(void) {
    int row = 2 + (currMemCell / 10);
    int col = 2 + (5 * (currMemCell % 10) + (currMemCell % 10));
    
    mt_gotoXY(col, row);
    
    short int currentValue;
    sc_memoryGet(currMemCell, &currentValue);
    
    char displayValue[6];
    getMemoryDisplayValue(currentValue, displayValue);
    
    printf("%s", displayValue);
    mt_gotoXY(col, row);
    
    char input[10] = {0};
    int pos = 0;
    char ch;
    
    while (1) {
        read(STDIN_FILENO, &ch, 1);
        
        if (ch == '\n') {
            input[pos] = '\0';
            break;
        } else if (ch == 27) { // ESC
            return 0;
        } else if (ch == 127 || ch == 8) { // Backspace
            if (pos > 0) {
                pos--;
                printf("\b \b");
            }
        } else if (isxdigit(ch) || ch == '+' || (pos == 0 && ch == '+')) {
            if (pos < 5) {
                input[pos++] = ch;
                printf("%c", ch);
            }
        }
    }
    
    if (strlen(input) > 0) {
        short int newValue;
        if (setMemoryFromDisplayValue(input, &newValue) == 0) {
            sc_memorySet(currMemCell, newValue);
        }
    }
    
    editMode = false;
    return 0;
}

/// Редактирование аккумулятора InPlace
int editAccumulatorInPlace(void) {
    mt_gotoXY(71, 2);
    printf("%04X", sc_accumulator);
    mt_gotoXY(71, 2);
    
    char input[10] = {0};
    int pos = 0;
    char ch;
    
    while (1) {
        read(STDIN_FILENO, &ch, 1);
        
        if (ch == '\n') {
            input[pos] = '\0';
            break;
        } else if (ch == 27) {
            return 0;
        } else if (ch == 127 || ch == 8) {
            if (pos > 0) {
                pos--;
                printf("\b \b");
            }
        } else if (isxdigit(ch)) {
            if (pos < 4) {
                input[pos++] = ch;
                printf("%c", ch);
            }
        }
    }
    
    if (strlen(input) > 0) {
        long int value = strtol(input, NULL, 16);
        if (value <= 0xFFFF) {
            sc_accumulator = (short int)value;
        }
    }
    
    editMode = false;
    currentEditTarget = TARGET_MEMORY;
    return 0;
}

/// Редактирование счетчика команд InPlace
int editCounterInPlace(void) {
    mt_gotoXY(71, 5);
    printf("%04X", sc_instructionCounter);
    mt_gotoXY(71, 5);
    
    char input[10] = {0};
    int pos = 0;
    char ch;
    
    while (1) {
        read(STDIN_FILENO, &ch, 1);
        
        if (ch == '\n') {
            input[pos] = '\0';
            break;
        } else if (ch == 27) {
            return 0;
        } else if (ch == 127 || ch == 8) {
            if (pos > 0) {
                pos--;
                printf("\b \b");
            }
        } else if (isxdigit(ch)) {
            if (pos < 2) {
                input[pos++] = ch;
                printf("%c", ch);
            }
        }
    }
    
    if (strlen(input) > 0) {
        long int value = strtol(input, NULL, 16);
        if (value <= 0x63) {
            sc_instructionCounter = (uint8_t)value;
        }
    }
    
    editMode = false;
    currentEditTarget = TARGET_MEMORY;
    return 0;
}

/// Обработка клавиши ENTER - переход в режим редактирования
int ui_handleEnter(void) {
    if (!editMode) {
        editMode = true;
        currentEditTarget = TARGET_MEMORY;
        return editMemoryInPlace();
    }
    return 0;
}

/// Обработка клавиши F5 - редактирование аккумулятора
int ui_handleF5(void) {
    if (!editMode) {
        editMode = true;
        currentEditTarget = TARGET_ACCUMULATOR;
        return editAccumulatorInPlace();
    }
    return 0;
}

/// Обработка клавиши F6 - редактирование счетчика команд
int ui_handleF6(void) {
    if (!editMode) {
        editMode = true;
        currentEditTarget = TARGET_COUNTER;
        return editCounterInPlace();
    }
    return 0;
}

int ui_saveMemory(void) {
    if (editMode) return 0;
    
    char filename[102];
    printf("Saving file...\n");
    printf("Enter the file name to save > ");
    mt_setFGcolor(SOFT_GREEN);
    fgets(filename, 102, stdin);
    mt_setDefaultColorSettings();
    
    if (filename[strlen(filename) - 1] != '\n') {
        printf("\033[38;5;%dmThe file name is too long. The length is trimmed to the first 100 characters.\033[0m\n", BLUE);
        clearBuffIn();
    } else {
        filename[strlen(filename) - 1] = '\0';
    }
    
    if (sc_memorySave(filename)) {
        ui_messageOutput((char *)"Failed to save memory", RED);
        return -1;
    } else {
        ui_messageOutput((char *)"Successful save", GREEN);
    }
    return 0;
}

int ui_loadMemory(void) {
    if (editMode) return 0;
    
    char filename[102];
    printf("Loading file...\n");
    printf("Enter the file name to load > ");
    mt_setFGcolor(SOFT_GREEN);
    fgets(filename, 102, stdin);
    mt_setDefaultColorSettings();
    
    if (filename[strlen(filename) - 1] != '\n') {
        ui_messageOutput((char *)"The name of the file to open is too long (up to 100 characters are allowed)", BLUE);
        clearBuffIn();
        return -1;
    }
    filename[strlen(filename) - 1] = '\0';
    
    if (sc_memoryLoad(filename)) {
        ui_messageOutput((char *)"Failed to load memory", RED);
        return -1;
    } else {
        ui_messageOutput((char *)"Successful load", GREEN);
    }
    return 0;
}

int ui_reset(void) {
    if (editMode) return 0;
    
    sc_regInit();
    sc_instructionCounter = 0;
    ui_messageOutput((char *)"System reset", GREEN);
    return 0;
}

/// Получение строкового представления значения ячейки памяти
int getMemoryDisplayValue(short int value, char *output) {
    if ((value >> 14) & 1)
        sprintf(output, " %04X", value & (~(1 << 14)));
    else
        sprintf(output, "+%04X", value);
    return 0;
}

/// Установка значения ячейки памяти из строки
int setMemoryFromDisplayValue(const char *input, short int *value) {
    long int number;
    if (input[0] == '+') {
        number = strtol(&input[1], NULL, 16);
        if (number > 0x3FFF) return -1;
        *value = (short int)number;
    } else {
        number = strtol(input, NULL, 16);
        if (number > 0x3FFF) return -1;
        *value = (short int)((1 << 14) | number);
    }
    return 0;
}

/// Отрисовка "боксов"
int drawingBoxes(void) {
    if (bc_box(1, 1, 61, 12)) return -1;
    if (bc_box(62, 1, 22, 3)) return -1;
    if (bc_box(62, 4, 22, 3)) return -1;
    if (bc_box(62, 7, 22, 3)) return -1;
    if (bc_box(62, 10, 22, 3)) return -1;
    if (bc_box(1, 13, 52, 10)) return -1;
    if (bc_box(53, 13, 31, 10)) return -1;
    return 0;
}

/// Отрисовка заголовков и текста
int drawingTexts(void) {
    mt_gotoXY(30, 1);
    printf(" Memory ");
    mt_gotoXY(66, 1);
    printf(" accumulator ");
    mt_gotoXY(63, 4);
    printf(" instructionCounter ");
    mt_gotoXY(67, 7);
    printf(" Operation ");
    mt_gotoXY(69, 10);
    printf(" Flags ");
    mt_gotoXY(54, 13);
    printf(" Keys: ");
    
    char* hotK[] = {(char *)"l  - load",
                    (char *)"s  - save",
                    (char *)"r  - run",
                    (char *)"t  - step",
                    (char *)"i  - reset",
                    (char *)"F5 - accumulator",
                    (char *)"F6 - instructionCounter"};
    
    int hotK_count = sizeof(hotK) / sizeof(hotK[0]);
    for (int i = 0; i < hotK_count; ++i) {
        mt_gotoXY(54, i + 14);
        printf("%s", hotK[i]);
    }
    return 0;
}

/// Отрисовка памяти
int drawingMemory(void) {
    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 10; ++j) {
            mt_gotoXY(2 + (5 * j + j), 2 + i);
            short int tmp;
            sc_memoryGet(i * 10 + j, &tmp);
            if ((i * 10 + j) == currMemCell && !editMode)
                mt_setBGcolor(GREEN);
            if ((tmp >> 14) & 1)
                printf(" %04X", tmp & (~(1 << 14)));
            else
                printf("+%04X", tmp);
            mt_setDefaultColorSettings();
        }
    }
    return 0;
}

/// Отрисовка accumulator
int drawingAccumulator(void) {
    mt_gotoXY(71, 2);
    printf("%04X", sc_accumulator);
    return 0;
}

/// Отрисовка Operation
int drawingOperation(void) {
    mt_gotoXY(71, 7);
    return 0;
}

/// Отрисовка instructionCounter
int drawingInstructionCounter(void) {
    mt_gotoXY(71, 5);
    printf("%04X", sc_instructionCounter);
    return 0;
}

/// Отрисовка флагов
int drawingFlags(void) {
    char tmp[] = {'O', 'Z', 'M', 'I', 'C'};
    for (int i = 0; i < SC_REG_SIZE; ++i) {
        bool value;
        if (sc_regGet(i, &value))
            return -1;
        mt_gotoXY(68 + (i * 2), 11);
        if (value) {
            mt_setFGcolor(PEACH);
            printf("%c", tmp[i]);
        } else {
            mt_setFGcolor(GRAY);
            printf("%c", tmp[i]);
        }
        mt_setDefaultColorSettings();
    }
    return 0;
}

/// Отрисовка "BigChar'ов"
int drawingBigChar(void) {
    short int tmp;
    sc_memoryGet(currMemCell, &tmp);
    if (!((tmp >> 14) & 1))
        bc_printBigChar(bc[16], 2, 14, GREEN, DEFAULT);
    
    tmp &= 0x3FFF;
    for (int i = 0; i < 4; ++i) {
        int ch = (tmp & (0xF << (4 * (3 - i)))) >> (4 * (3 - i));
        bc_printBigChar(bc[ch], 2 + 8 * (i + 1) + 2 * (i + 1), 14, GREEN, DEFAULT);
    }
    return 0;
}

bool checkCorrectInput(const char buffer[10]) {
    int i;
    size_t len = strlen(buffer);
    
    if (buffer[0] == '+') {
        if (len == 2 || len > 6)
            return false;
        i = 1;
    } else {
        i = 0;
        if (len == 1 || len > 5)
            return false;
    }
    
    for (; i < (int)(len - 1); ++i) {
        if (!(isxdigit(buffer[i])))
            return false;
    }
    return true;
}

int ui_messageOutput(char *str, enum colors color) {
    printf("\033[38;5;%dm%s\033[0m", color, str);
    rk_pause(25);
    return 0;
}

int clearBuffIn(void) {
    int c;
    do {
        c = getchar();
    } while (c != '\n' && c != '\0');
    return 0;
}

// Добавьте в конец файла myUI.c:

int ui_start(void) {
    if (editMode) return 0;
    sc_start();
    ui_messageOutput((char *)"Model started", GREEN);
    return 0;
}

int ui_stop(void) {
    if (editMode) return 0;
    sc_stop();
    ui_messageOutput((char *)"Model stopped", GREEN);
    return 0;
}

int ui_step(void) {
    if (editMode) return 0;
    sc_step();
    ui_update();
    ui_messageOutput((char *)"Step executed", GREEN);
    return 0;
}