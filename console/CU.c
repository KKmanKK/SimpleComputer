#include "console.h"

/* Управляющее устройство (Control Unit)
   Выполняет один машинный цикл:
   1. Fetch - выборка команды из памяти
   2. Decode - декодирование команды
   3. Execute - исполнение (своё или через ALU)
   4. Writeback - обновление счётчика команд и экрана */
void CU(void) {
    /* ===== 1. FETCH (выборка команды) ===== */
    int ic = 0;
    sc_icounterGet(&ic);                     /* читаем текущий адрес */

    int cell_value = 0;
    if (sc_memoryGet(ic, &cell_value) != 0) {
        /* Ошибка: неверный адрес памяти */
        sc_regSet(FLAG_MEMORY_OUT_OF_BOUNDS, 1);
        sc_regSet(FLAG_CLOCK_PULSES_IGNORED, 1);
        printFlags();
        fflush(stdout);
        return;
    }

    /* ===== 2. DECODE (декодирование) ===== */
    int sign = 0, command = 0, operand = 0;
    if (sc_commandDecode(cell_value, &sign, &command, &operand) != 0) {
        /* Ошибка: не удалось декодировать команду */
        sc_regSet(FLAG_COMMAND_NOT_FOUND, 1);
        sc_regSet(FLAG_CLOCK_PULSES_IGNORED, 1);
        printFlags();
        fflush(stdout);
        return;
    }

    /* ===== 3. VALIDATE (проверка команды) ===== */
    if (sc_commandValidate(command) != 0) {
        /* Ошибка: неизвестная команда */
        sc_regSet(FLAG_COMMAND_NOT_FOUND, 1);
        sc_regSet(FLAG_CLOCK_PULSES_IGNORED, 1);
        printFlags();
        fflush(stdout);
        return;
    }

    /* ===== 4. EXECUTE (исполнение) ===== */
    int next_ic = (ic + 1) % MEMORY_SIZE;    /* следующий адрес (по умолчанию) */
    int acc = 0;
    sc_accumulatorGet(&acc);
    int ovf = 0;
    sc_regGet(FLAG_OVERFLOW, &ovf);

    switch (command) {
    case 40:  /* JUMP - безусловный переход */
        next_ic = operand;
        break;

    case 41:  /* JNEG - переход если аккумулятор < 0 */
        if (acc < 0) next_ic = operand;
        break;

    case 42:  /* JZ - переход если аккумулятор == 0 */
        if (acc == 0) next_ic = operand;
        break;

    case 43:  /* HALT - останов процессора */
        sc_regSet(FLAG_CLOCK_PULSES_IGNORED, 1);
        printCommand();
        printFlags();
        fflush(stdout);
        return;  /* не обновляем IC и не перерисовываем остальное */

    case 55:  /* JNS - переход если аккумулятор > 0 */
        if (acc > 0) next_ic = operand;
        break;

    case 56:  /* JC - переход если установлен флаг переполнения */
        if (ovf) next_ic = operand;
        break;

    case 57:  /* JNC - переход если флаг переполнения не установлен */
        if (!ovf) next_ic = operand;
        break;

    case 58:  /* JP - переход если аккумулятор чётный */
        if (acc % 2 == 0) next_ic = operand;
        break;

    case 59:  /* JNP - переход если аккумулятор нечётный */
        if (acc % 2 != 0) next_ic = operand;
        break;

    default:
        /* Все остальные команды (арифметика, логика, ввод/вывод) */
        ALU(command, operand);
        break;
    }

    /* ===== 5. WRITEBACK (обновление состояния) ===== */
    sc_icounterSet(next_ic);               /* обновляем счётчик команд */

    /* Обновление экрана */
    printCommand();      /* блок "Command" */
    printFlags();        /* блок "Flags" */
    printAccumulator();  /* блок "Accumulator" */
    printCounters();     /* блок "Instruction Counter" */
    printBigChars();     /* большие цифры аккумулятора */
    fflush(stdout);
}