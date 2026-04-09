#include "console.h"
#include <stdio.h>

/*
 * ALU — Arithmetic-Logic Unit.
 *
 * Implements one ALU clock cycle for all command codes < 80 (0x50)
 * plus two user-defined functions (codes 80, 81).
 *
 * Jump commands (40-42, 55-59) and HALT (43) are no-ops here;
 * the control-flow logic lives in CU.
 *
 * Returns  0 on success, -1 on error (flags already set).
 */
int ALU(int command, int operand) {
    int acc = 0;
    sc_accumulatorGet(&acc);
    int mem_val = 0;

    /* Validate operand for all memory-accessing commands */
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

    /* ── I/O ────────────────────────────────────────────────────────── */

    case 10: { /* READ — read hex value from user, store in memory[operand] */
        int val = 0;
        mt_setcursorvisible(1);
        mt_gotoXY(PROMPT_ROW, 1);
        printf("Input [%3d]: ", operand);
        fflush(stdout);
        rk_readvalue(&val, 0);           /* handles its own terminal mode */
        rk_mytermregime(0, 0, 1, 0, 1); /* back to raw */
        mt_gotoXY(PROMPT_ROW, 1);
        printf("%-*s", MIN_COLS, "");    /* clear prompt row */
        fflush(stdout);
        mt_setcursorvisible(0);
        sc_memorySet(operand, val);
        printTerm(operand, 0);           /* show result in IN-OUT */
        /* refresh the memory cell on screen */
        printCell(operand, operand == g_selected ? C_BLACK : C_DEFAULT,
                           operand == g_selected ? C_WHITE : C_DEFAULT);
        break;
    }

    case 11: /* WRITE — output memory[operand] to IN-OUT block */
        printTerm(operand, 0);
        break;

    /* ── Load / Store ───────────────────────────────────────────────── */

    case 20: /* LOAD — accumulator = memory[operand] */
        sc_memoryGet(operand, &mem_val);
        sc_accumulatorSet(mem_val);
        break;

    case 21: /* STORE — memory[operand] = accumulator */
        sc_memorySet(operand, acc);
        printCell(operand, operand == g_selected ? C_BLACK : C_DEFAULT,
                           operand == g_selected ? C_WHITE : C_DEFAULT);
        break;

    /* ── Arithmetic ─────────────────────────────────────────────────── */

    case 30: { /* ADD */
        sc_memoryGet(operand, &mem_val);
        int result = acc + mem_val;
        sc_regSet(FLAG_OVERFLOW,
                  (result > ACC_MAX_VALUE || result < ACC_MIN_VALUE) ? 1 : 0);
        sc_accumulatorSet(result);
        break;
    }

    case 31: { /* SUB */
        sc_memoryGet(operand, &mem_val);
        int result = acc - mem_val;
        sc_regSet(FLAG_OVERFLOW,
                  (result > ACC_MAX_VALUE || result < ACC_MIN_VALUE) ? 1 : 0);
        sc_accumulatorSet(result);
        break;
    }

    case 32: { /* DIVIDE */
        sc_memoryGet(operand, &mem_val);
        if (mem_val == 0) {
            sc_regSet(FLAG_DIVISION_BY_ZERO, 1);
            sc_regSet(FLAG_CLOCK_PULSES_IGNORED, 1);
            return -1;
        }
        sc_regSet(FLAG_DIVISION_BY_ZERO, 0);
        sc_accumulatorSet(acc / mem_val);
        break;
    }

    case 33: { /* MUL */
        sc_memoryGet(operand, &mem_val);
        int result = acc * mem_val;
        sc_regSet(FLAG_OVERFLOW,
                  (result > ACC_MAX_VALUE || result < ACC_MIN_VALUE) ? 1 : 0);
        sc_accumulatorSet(result);
        break;
    }

    /* ── Jump commands — control flow handled by CU, ALU is a no-op ─── */

    case 40: /* JUMP  */
    case 41: /* JNEG  */
    case 42: /* JZ    */
    case 55: /* JNS   */
    case 56: /* JC    */
    case 57: /* JNC   */
    case 58: /* JP    */
    case 59: /* JNP   */
        break;

    case 43: /* HALT */
        sc_regSet(FLAG_CLOCK_PULSES_IGNORED, 1);
        break;

    /* ── Bitwise ────────────────────────────────────────────────────── */

    case 51: /* NOT — bitwise NOT of accumulator, result stored in mem */
        sc_accumulatorSet(~acc);
        sc_memorySet(operand, ~acc);
        break;

    case 52: /* AND */
        sc_memoryGet(operand, &mem_val);
        sc_accumulatorSet(acc & mem_val);
        break;

    case 53: /* OR */
        sc_memoryGet(operand, &mem_val);
        sc_accumulatorSet(acc | mem_val);
        break;

    case 54: /* XOR */
        sc_memoryGet(operand, &mem_val);
        sc_accumulatorSet(acc ^ mem_val);
        break;

    /* ── Shift / Rotate ─────────────────────────────────────────────── */

    case 60: /* SHL — shift left by mem[operand] bits */
        sc_memoryGet(operand, &mem_val);
        sc_accumulatorSet(acc << mem_val);
        break;

    case 61: /* SHR — shift right by mem[operand] bits */
        sc_memoryGet(operand, &mem_val);
        sc_accumulatorSet(acc >> mem_val);
        break;

    case 62: { /* RCL — rotate left */
        sc_memoryGet(operand, &mem_val);
        int bits = 16;
        mem_val %= bits;
        sc_accumulatorSet((acc << mem_val) | (acc >> (bits - mem_val)));
        break;
    }

    case 63: { /* RCR — rotate right */
        sc_memoryGet(operand, &mem_val);
        int bits = 16;
        mem_val %= bits;
        sc_accumulatorSet((acc >> mem_val) | (acc << (bits - mem_val)));
        break;
    }

    case 64: /* NEG — two's complement negation of mem[operand] */
        sc_memoryGet(operand, &mem_val);
        sc_accumulatorSet(~mem_val + 1);
        break;

    case 65: { /* ADDC — acc = mem[operand] + mem[acc] (indirect) */
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

    case 66: { /* SUBC — acc = mem[operand] - mem[acc] (indirect) */
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

    case 67: { /* LOGLC — logical shift left: mem[operand] << acc */
        sc_memoryGet(operand, &mem_val);
        sc_accumulatorSet(mem_val << acc);
        break;
    }

    case 68: { /* LOGRC — logical shift right: mem[operand] >> acc */
        sc_memoryGet(operand, &mem_val);
        sc_accumulatorSet(mem_val >> acc);
        break;
    }

    case 69: { /* RCCL — rotate mem[operand] left by acc */
        sc_memoryGet(operand, &mem_val);
        int bits = 16;
        int n = acc % bits;
        sc_accumulatorSet((mem_val << n) | (mem_val >> (bits - n)));
        break;
    }

    case 70: { /* RCCR — rotate mem[operand] right by acc */
        sc_memoryGet(operand, &mem_val);
        int bits = 16;
        int n = acc % bits;
        sc_accumulatorSet((mem_val >> n) | (mem_val << (bits - n)));
        break;
    }

    /* ── Move (indirect addressing) ─────────────────────────────────── */

    case 71: { /* MOVA — mem[acc] = mem[operand] */
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

    case 72: { /* MOVR — mem[operand] = mem[acc] */
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
        /* Double-indirect variants — complex, treat as NOP for now */
        break;

    /* ── User-defined functions ─────────────────────────────────────── */

    case 80: { /* SQR — accumulator = acc * acc */
        int result = acc * acc;
        sc_regSet(FLAG_OVERFLOW,
                  (result > ACC_MAX_VALUE || result < ACC_MIN_VALUE) ? 1 : 0);
        sc_accumulatorSet(result);
        break;
    }

    case 81: { /* ABS — accumulator = |acc| */
        sc_accumulatorSet(acc < 0 ? -acc : acc);
        break;
    }

    default:
        sc_regSet(FLAG_COMMAND_NOT_FOUND, 1);
        sc_regSet(FLAG_CLOCK_PULSES_IGNORED, 1);
        return -1;
    }

    return 0;
}
