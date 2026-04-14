#include "console.h"

/* Арифметико-логическое устройство (ALU)
   Выполняет все арифметические, логические и операции ввода-вывода
   command - код операции, operand - адрес операнда в памяти */
int ALU(int command, int operand) {
    int acc = 0;
    sc_accumulatorGet(&acc);          /* читаем текущее значение аккумулятора */
    int mem_val = 0;

    /* Определяем, нужен ли доступ к памяти для этой команды */
    int needs_mem = (command == 10 || command == 11 ||
                     command == 20 || command == 21 ||
                     (command >= 30 && command <= 33) ||
                     (command >= 51 && command <= 54) ||
                     (command >= 60 && command <= 76));

    /* Проверка границ памяти, если нужен доступ */
    if (needs_mem && (operand < 0 || operand >= MEMORY_SIZE)) {
        sc_regSet(FLAG_MEMORY_OUT_OF_BOUNDS, 1);
        sc_regSet(FLAG_CLOCK_PULSES_IGNORED, 1);
        return -1;
    }

    switch (command) {
    /* ==================== ВВОД/ВЫВОД ==================== */
    case 10:  /* READ - ввод числа с клавиатуры в память */
        int val = 0;
        mt_setcursorvisible(1);
        mt_gotoXY(PROMPT_ROW, 1);
        printf("Input [%3d]: ", operand);
        fflush(stdout);
        rk_readvalue(&val, 0);                     /* чтение числа */
        rk_mytermregime(0, 0, 1, 0, 1);            /* возврат в raw режим */
        mt_gotoXY(PROMPT_ROW, 1);
        printf("%-*s", MIN_COLS, "");              /* очистка строки ввода */
        fflush(stdout);
        mt_setcursorvisible(0);
        sc_memorySet(operand, val);                /* сохранение в память */
        printTerm(operand, 0);                     /* вывод в блок IN/OUT */
        printCell(operand, operand == g_selected ? C_BLACK : C_DEFAULT,
                           operand == g_selected ? C_WHITE : C_DEFAULT);
        break;

    case 11:  /* WRITE - вывод значения ячейки на экран */
        printTerm(operand, 0);
        break;

    /* ==================== ЗАГРУЗКА/СОХРАНЕНИЕ ==================== */
    case 20:  /* LOAD - загрузка из памяти в аккумулятор */
        sc_memoryGet(operand, &mem_val);
        sc_accumulatorSet(mem_val);
        break;

    case 21:  /* STORE - сохранение аккумулятора в память */
        sc_memorySet(operand, acc);
        printCell(operand, operand == g_selected ? C_BLACK : C_DEFAULT,
                           operand == g_selected ? C_WHITE : C_DEFAULT);
        break;

    /* ==================== АРИФМЕТИЧЕСКИЕ ОПЕРАЦИИ ==================== */
    case 30: {  /* ADD - сложение */
        sc_memoryGet(operand, &mem_val);
        int result = acc + mem_val;
        sc_regSet(FLAG_OVERFLOW,
                  (result > ACC_MAX_VALUE || result < ACC_MIN_VALUE) ? 1 : 0);
        sc_accumulatorSet(result);
        break;
    }

    case 31: {  /* SUB - вычитание */
        sc_memoryGet(operand, &mem_val);
        int result = acc - mem_val;
        sc_regSet(FLAG_OVERFLOW,
                  (result > ACC_MAX_VALUE || result < ACC_MIN_VALUE) ? 1 : 0);
        sc_accumulatorSet(result);
        break;
    }

    case 32:  /* DIVIDE - деление (с проверкой деления на 0) */
        sc_memoryGet(operand, &mem_val);
        if (mem_val == 0) {
            sc_regSet(FLAG_DIVISION_BY_ZERO, 1);
            sc_regSet(FLAG_CLOCK_PULSES_IGNORED, 1);
            return -1;
        }
        sc_regSet(FLAG_DIVISION_BY_ZERO, 0);
        sc_accumulatorSet(acc / mem_val);
        break;

    case 33: {  /* MUL - умножение */
        sc_memoryGet(operand, &mem_val);
        int result = acc * mem_val;
        sc_regSet(FLAG_OVERFLOW,
                  (result > ACC_MAX_VALUE || result < ACC_MIN_VALUE) ? 1 : 0);
        sc_accumulatorSet(result);
        break;
    }

    /* ==================== КОМАНДЫ УПРАВЛЕНИЯ ==================== */
    case 40: case 41: case 42:  /* JUMP, JNEG, JZ */
    case 55: case 56: case 57: case 58: case 59:  /* JNS, JC, JNC, JP, JNP */
        break;  /* обрабатываются в CU, здесь ничего не делаем */

    case 43:  /* HALT - останов процессора */
        sc_regSet(FLAG_CLOCK_PULSES_IGNORED, 1);
        break;

    /* ==================== ЛОГИЧЕСКИЕ ОПЕРАЦИИ ==================== */
    case 51:  /* NOT - побитовое НЕ */
        sc_accumulatorSet(~acc);
        sc_memorySet(operand, ~acc);
        break;

    case 52:  /* AND - побитовое И */
        sc_memoryGet(operand, &mem_val);
        sc_accumulatorSet(acc & mem_val);
        break;

    case 53:  /* OR - побитовое ИЛИ */
        sc_memoryGet(operand, &mem_val);
        sc_accumulatorSet(acc | mem_val);
        break;

    case 54:  /* XOR - побитовое исключающее ИЛИ */
        sc_memoryGet(operand, &mem_val);
        sc_accumulatorSet(acc ^ mem_val);
        break;

    /* ==================== СДВИГИ И ВРАЩЕНИЯ ==================== */
    case 60:  /* SHL - логический сдвиг влево */
        sc_memoryGet(operand, &mem_val);
        sc_accumulatorSet(acc << mem_val);
        break;

    case 61:  /* SHR - логический сдвиг вправо */
        sc_memoryGet(operand, &mem_val);
        sc_accumulatorSet(acc >> mem_val);
        break;

    case 62: {  /* RCL - циклический сдвиг влево */
        sc_memoryGet(operand, &mem_val);
        int bits = 16;
        mem_val %= bits;
        sc_accumulatorSet((acc << mem_val) | (acc >> (bits - mem_val)));
        break;
    }

    case 63: {  /* RCR - циклический сдвиг вправо */
        sc_memoryGet(operand, &mem_val);
        int bits = 16;
        mem_val %= bits;
        sc_accumulatorSet((acc >> mem_val) | (acc << (bits - mem_val)));
        break;
    }

    case 64:  /* NEG - арифметическое отрицание (дополнительный код) */
        sc_memoryGet(operand, &mem_val);
        sc_accumulatorSet(~mem_val + 1);
        break;

    /* ==================== КОСВЕННАЯ АДРЕСАЦИЯ ==================== */
    case 65: {  /* ADDC - сложение с косвенной адресацией */
        sc_memoryGet(operand, &mem_val);
        int addr = acc;
        if (addr < 0 || addr >= MEMORY_SIZE) {
            sc_regSet(FLAG_MEMORY_OUT_OF_BOUNDS, 1);
            sc_regSet(FLAG_CLOCK_PULSES_IGNORED, 1);
            return -1;
        }
        int indirect;
        sc_memoryGet(addr, &indirect);
        sc_accumulatorSet(mem_val + indirect);
        break;
    }

    case 66: {  /* SUBC - вычитание с косвенной адресацией */
        sc_memoryGet(operand, &mem_val);
        int addr = acc;
        if (addr < 0 || addr >= MEMORY_SIZE) {
            sc_regSet(FLAG_MEMORY_OUT_OF_BOUNDS, 1);
            sc_regSet(FLAG_CLOCK_PULSES_IGNORED, 1);
            return -1;
        }
        int indirect;
        sc_memoryGet(addr, &indirect);
        sc_accumulatorSet(mem_val - indirect);
        break;
    }

    case 67:  /* LOGLC - логический сдвиг влево операнда на acc */
        sc_memoryGet(operand, &mem_val);
        sc_accumulatorSet(mem_val << acc);
        break;

    case 68:  /* LOGRC - логический сдвиг вправо операнда на acc */
        sc_memoryGet(operand, &mem_val);
        sc_accumulatorSet(mem_val >> acc);
        break;

    case 69: {  /* RCCL - циклический сдвиг операнда влево на acc */
        sc_memoryGet(operand, &mem_val);
        int bits = 16;
        int n = acc % bits;
        sc_accumulatorSet((mem_val << n) | (mem_val >> (bits - n)));
        break;
    }

    case 70: {  /* RCCR - циклический сдвиг операнда вправо на acc */
        sc_memoryGet(operand, &mem_val);
        int bits = 16;
        int n = acc % bits;
        sc_accumulatorSet((mem_val >> n) | (mem_val << (bits - n)));
        break;
    }

    /* ==================== ПЕРЕСЫЛКИ С КОСВЕННОЙ АДРЕСАЦИЕЙ ==================== */
    case 71: {  /* MOVA - mem[acc] = mem[operand] */
        sc_memoryGet(operand, &mem_val);
        int dst = acc;
        if (dst < 0 || dst >= MEMORY_SIZE) {
            sc_regSet(FLAG_MEMORY_OUT_OF_BOUNDS, 1);
            sc_regSet(FLAG_CLOCK_PULSES_IGNORED, 1);
            return -1;
        }
        sc_memorySet(dst, mem_val);
        printCell(dst, dst == g_selected ? C_BLACK : C_DEFAULT,
                       dst == g_selected ? C_WHITE : C_DEFAULT);
        break;
    }

    case 72: {  /* MOVR - mem[operand] = mem[acc] */
        int src = acc;
        if (src < 0 || src >= MEMORY_SIZE) {
            sc_regSet(FLAG_MEMORY_OUT_OF_BOUNDS, 1);
            sc_regSet(FLAG_CLOCK_PULSES_IGNORED, 1);
            return -1;
        }
        sc_memoryGet(src, &mem_val);
        sc_memorySet(operand, mem_val);
        printCell(operand, operand == g_selected ? C_BLACK : C_DEFAULT,
                           operand == g_selected ? C_WHITE : C_DEFAULT);
        break;
    }

    case 73: case 74: case 75: case 76:  /* MOVCA, MOVCR, ADDC, SUBC (двойная косвенная) */
        break;  /* не реализованы */

    /* ==================== ПОЛЬЗОВАТЕЛЬСКИЕ ФУНКЦИИ ==================== */
    case 80: {  /* SQR - квадрат числа */
        int result = acc * acc;
        sc_regSet(FLAG_OVERFLOW,
                  (result > ACC_MAX_VALUE || result < ACC_MIN_VALUE) ? 1 : 0);
        sc_accumulatorSet(result);
        break;
    }

    case 81:  /* ABS - абсолютное значение */
        sc_accumulatorSet(acc < 0 ? -acc : acc);
        break;

    /* ==================== НЕИЗВЕСТНАЯ КОМАНДА ==================== */
    default:
        sc_regSet(FLAG_COMMAND_NOT_FOUND, 1);
        sc_regSet(FLAG_CLOCK_PULSES_IGNORED, 1);
        return -1;
    }

    return 0;
}