#include "mySimpleComputer.h"

/* Глобальные переменные эмулятора */
int sc_memory[MEMORY_SIZE];     /* оперативная память (128 ячеек) */
int sc_flags;                   /* регистр флагов (битовая маска) */
int sc_accumulator;             /* аккумулятор - главный регистр */
int sc_icounter;                /* счётчик команд - адрес текущей команды */

/* Список допустимых команд процессора */
static const int valid_commands[] = {
    10, 11,                          /* READ, WRITE - ввод/вывод */
    20, 21,                          /* LOAD, STORE - загрузка/сохранение */
    30, 31, 32, 33,                  /* ADD, SUB, DIVIDE, MUL - арифметика */
    40, 41, 42, 43,                  /* JUMP, JNEG, JZ, HALT - управление */
    51, 52, 53, 54, 55, 56, 57, 58, 59, /* логические и условные переходы */
    60, 61, 62, 63, 64, 65, 66,      /* сдвиги и вращения */
    67, 68, 69, 70,                  /* логические сдвиги */
    71, 72, 73, 74, 75, 76,          /* пересылки с косвенной адресацией */
    80, 81                           /* пользовательские: SQR, ABS */
};

/* ==================== Функции работы с памятью ==================== */

/* Инициализация памяти - обнуление всех ячеек */
int sc_memoryInit(void) {
    for (int i = 0; i < MEMORY_SIZE; i++)
        sc_memory[i] = 0;
    return 0;
}

/* Запись значения в ячейку памяти с проверкой границ и диапазона */
int sc_memorySet(int address, int value) {
    if (address < 0 || address >= MEMORY_SIZE)
        return -1;
    if (value < MEMORY_MIN_VALUE || value > MEMORY_MAX_VALUE)
        return -1;
    sc_memory[address] = value;
    return 0;
}

/* Чтение значения из ячейки памяти */
int sc_memoryGet(int address, int *value) {
    if (address < 0 || address >= MEMORY_SIZE)
        return -1;
    if (value == NULL)
        return -1;
    *value = sc_memory[address];
    return 0;
}

/* Сохранение всей памяти в бинарный файл */
int sc_memorySave(char *filename) {
    if (filename == NULL)
        return -1;
    FILE *file = fopen(filename, "wb");
    if (file == NULL)
        return -1;
    size_t written = fwrite(sc_memory, sizeof(int), MEMORY_SIZE, file);
    fclose(file);
    return (written == MEMORY_SIZE) ? 0 : -1;
}

/* Загрузка памяти из бинарного файла */
int sc_memoryLoad(char *filename) {
    if (filename == NULL)
        return -1;
    FILE *file = fopen(filename, "rb");
    if (file == NULL)
        return -1;
    int tmp[MEMORY_SIZE];
    size_t read = fread(tmp, sizeof(int), MEMORY_SIZE, file);
    fclose(file);
    if (read != MEMORY_SIZE)
        return -1;
    for (int i = 0; i < MEMORY_SIZE; i++)
        sc_memory[i] = tmp[i];
    return 0;
}

/* ==================== Функции работы с регистром флагов ==================== */

/* Инициализация регистра флагов - все флаги = 0 */
int sc_regInit(void) {
    sc_flags = 0;
    return 0;
}

/* Установка или сброс указанного флага (value: 1 - установить, 0 - сбросить) */
int sc_regSet(int flag, int value) {
    if (flag < 1 || flag > FLAG_COUNT)
        return -1;
    if (value)
        sc_flags |= (1 << (flag - 1));   /* установка бита */
    else
        sc_flags &= ~(1 << (flag - 1));  /* сброс бита */
    return 0;
}

/* Получение состояния указанного флага (0 или 1) */
int sc_regGet(int flag, int *value) {
    if (flag < 1 || flag > FLAG_COUNT)
        return -1;
    if (value == NULL)
        return -1;
    *value = (sc_flags >> (flag - 1)) & 0x1;  /* выделение бита */
    return 0;
}

/* ==================== Функции работы с аккумулятором ==================== */

/* Инициализация аккумулятора - установка в 0 */
int sc_accumulatorInit(void) {
    sc_accumulator = 0;
    return 0;
}

/* Установка значения аккумулятора с проверкой допустимого диапазона */
int sc_accumulatorSet(int value) {
    if (value < ACC_MIN_VALUE || value > ACC_MAX_VALUE)
        return -1;
    sc_accumulator = value;
    return 0;
}

/* Чтение значения аккумулятора */
int sc_accumulatorGet(int *value) {
    if (value == NULL)
        return -1;
    *value = sc_accumulator;
    return 0;
}

/* ==================== Функции работы со счётчиком команд ==================== */

/* Инициализация счётчика команд - установка в 0 */
int sc_icounterInit(void) {
    sc_icounter = 0;
    return 0;
}

/* Установка счётчика команд с проверкой границ памяти */
int sc_icounterSet(int value) {
    if (value < 0 || value >= MEMORY_SIZE)
        return -1;
    sc_icounter = value;
    return 0;
}

/* Чтение значения счётчика команд */
int sc_icounterGet(int *value) {
    if (value == NULL)
        return -1;
    *value = sc_icounter;
    return 0;
}

/* ==================== Функции кодирования/декодирования команд ==================== */

#define MAX_ENCODED 131071  /* максимальное закодированное значение (2^17 - 1) */

/* Кодирование команды в 32-битное значение:
   бит 16 - знак, биты 15-8 - код команды, биты 7-0 - операнд */
int sc_commandEncode(int sign, int command, int operand, int *value) {
    if (value == NULL)
        return -1;
    if (sign < 0 || sign > 1)
        return -1;
    if (command < 0 || command > 255)
        return -1;
    if (operand < 0 || operand > 255)
        return -1;
    *value = (sign << 16) | (command << 8) | operand;
    return 0;
}

/* Декодирование 32-битного значения в поля команды */
int sc_commandDecode(int value, int *sign, int *command, int *operand) {
    if (value < 0 || value > MAX_ENCODED)
        return -1;
    if (sign == NULL || command == NULL || operand == NULL)
        return -1;
    *sign    = (value >> 16) & 0x1;    /* выделяем бит 16 */
    *command = (value >>  8) & 0xFF;   /* выделяем биты 15-8 */
    *operand =  value        & 0xFF;   /* выделяем биты 7-0 */
    return 0;
}

/* Проверка, является ли код команды допустимым */
int sc_commandValidate(int command) {
    int n = sizeof(valid_commands) / sizeof(valid_commands[0]);
    for (int i = 0; i < n; i++) {
        if (valid_commands[i] == command)
            return 0;  /* команда найдена */
    }
    return -1;  /* команда не найдена */
}