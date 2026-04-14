#include "console.h"

int ALU(int command, int operand) {
    int acc = 0;
    sc_accumulatorGet(&acc);
    int mem_val = 0;

    int needs_mem = (command == 10 || command == 11 ||
                     command == 20 || command == 21 ||
                     (command >= 30 && command <= 33) ||
                     (command >= 51 && command <= 54) ||
                     (command >= 60 && command <= 76));

    if (needs_mem && (operand < 0 || operand >= MEMORY_SIZE)) {
        sc_regSet(FLAG_MEMORY_OUT_OF_BOUNDS, 1);
        sc_regSet(FLAG_CLOCK_PULSES_IGNORED, 1);
        return -1;
    }

    switch (command) {
    case 10:
        int val = 0;
        mt_setcursorvisible(1);
        mt_gotoXY(PROMPT_ROW, 1);
        printf("Input [%3d]: ", operand);
        fflush(stdout);
        rk_readvalue(&val, 0);
        rk_mytermregime(0, 0, 1, 0, 1);
        mt_gotoXY(PROMPT_ROW, 1);
        printf("%-*s", MIN_COLS, "");
        fflush(stdout);
        mt_setcursorvisible(0);
        sc_memorySet(operand, val);
        printTerm(operand, 0);
        printCell(operand, operand == g_selected ? C_BLACK : C_DEFAULT,
                           operand == g_selected ? C_WHITE : C_DEFAULT);
        break;

    case 11:
        printTerm(operand, 0);
        break;

    case 20:
        sc_memoryGet(operand, &mem_val);
        sc_accumulatorSet(mem_val);
        break;

    case 21:
        sc_memorySet(operand, acc);
        printCell(operand, operand == g_selected ? C_BLACK : C_DEFAULT,
                           operand == g_selected ? C_WHITE : C_DEFAULT);
        break;

    case 30: {
        sc_memoryGet(operand, &mem_val);
        int result = acc + mem_val;
        sc_regSet(FLAG_OVERFLOW,
                  (result > ACC_MAX_VALUE || result < ACC_MIN_VALUE) ? 1 : 0);
        sc_accumulatorSet(result);
        break;
    }

    case 31: {
        sc_memoryGet(operand, &mem_val);
        int result = acc - mem_val;
        sc_regSet(FLAG_OVERFLOW,
                  (result > ACC_MAX_VALUE || result < ACC_MIN_VALUE) ? 1 : 0);
        sc_accumulatorSet(result);
        break;
    }

    case 32:
        sc_memoryGet(operand, &mem_val);
        if (mem_val == 0) {
            sc_regSet(FLAG_DIVISION_BY_ZERO, 1);
            sc_regSet(FLAG_CLOCK_PULSES_IGNORED, 1);
            return -1;
        }
        sc_regSet(FLAG_DIVISION_BY_ZERO, 0);
        sc_accumulatorSet(acc / mem_val);
        break;

    case 33: {
        sc_memoryGet(operand, &mem_val);
        int result = acc * mem_val;
        sc_regSet(FLAG_OVERFLOW,
                  (result > ACC_MAX_VALUE || result < ACC_MIN_VALUE) ? 1 : 0);
        sc_accumulatorSet(result);
        break;
    }

    case 40: case 41: case 42: case 55: case 56: case 57: case 58: case 59:
        break;

    case 43:
        sc_regSet(FLAG_CLOCK_PULSES_IGNORED, 1);
        break;

    case 51:
        sc_accumulatorSet(~acc);
        sc_memorySet(operand, ~acc);
        break;

    case 52:
        sc_memoryGet(operand, &mem_val);
        sc_accumulatorSet(acc & mem_val);
        break;

    case 53:
        sc_memoryGet(operand, &mem_val);
        sc_accumulatorSet(acc | mem_val);
        break;

    case 54:
        sc_memoryGet(operand, &mem_val);
        sc_accumulatorSet(acc ^ mem_val);
        break;

    case 60:
        sc_memoryGet(operand, &mem_val);
        sc_accumulatorSet(acc << mem_val);
        break;

    case 61:
        sc_memoryGet(operand, &mem_val);
        sc_accumulatorSet(acc >> mem_val);
        break;

    case 62: {
        sc_memoryGet(operand, &mem_val);
        int bits = 16;
        mem_val %= bits;
        sc_accumulatorSet((acc << mem_val) | (acc >> (bits - mem_val)));
        break;
    }

    case 63: {
        sc_memoryGet(operand, &mem_val);
        int bits = 16;
        mem_val %= bits;
        sc_accumulatorSet((acc >> mem_val) | (acc << (bits - mem_val)));
        break;
    }

    case 64:
        sc_memoryGet(operand, &mem_val);
        sc_accumulatorSet(~mem_val + 1);
        break;

    case 65: {
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

    case 66: {
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

    case 67:
        sc_memoryGet(operand, &mem_val);
        sc_accumulatorSet(mem_val << acc);
        break;

    case 68:
        sc_memoryGet(operand, &mem_val);
        sc_accumulatorSet(mem_val >> acc);
        break;

    case 69: {
        sc_memoryGet(operand, &mem_val);
        int bits = 16;
        int n = acc % bits;
        sc_accumulatorSet((mem_val << n) | (mem_val >> (bits - n)));
        break;
    }

    case 70: {
        sc_memoryGet(operand, &mem_val);
        int bits = 16;
        int n = acc % bits;
        sc_accumulatorSet((mem_val >> n) | (mem_val << (bits - n)));
        break;
    }

    case 71: {
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

    case 72: {
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

    case 73: case 74: case 75: case 76:
        break;

    case 80: {
        int result = acc * acc;
        sc_regSet(FLAG_OVERFLOW,
                  (result > ACC_MAX_VALUE || result < ACC_MIN_VALUE) ? 1 : 0);
        sc_accumulatorSet(result);
        break;
    }

    case 81:
        sc_accumulatorSet(acc < 0 ? -acc : acc);
        break;

    default:
        sc_regSet(FLAG_COMMAND_NOT_FOUND, 1);
        sc_regSet(FLAG_CLOCK_PULSES_IGNORED, 1);
        return -1;
    }

    return 0;
}