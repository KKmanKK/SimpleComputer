#include "console.h"

/*
 * CU — Control Unit.
 *
 * Implements one control-unit clock cycle:
 *   1. Fetch:    read memory[IC]
 *   2. Decode:   sc_commandDecode
 *   3. Validate: sc_commandValidate
 *   4. Execute:  handle jump/halt directly, delegate the rest to ALU
 *   5. Writeback: update IC, refresh display
 */
void CU(void) {
    int ic = 0;
    sc_icounterGet(&ic);

    /* ── 1. Fetch ────────────────────────────────────────────────────── */
    int cell_value = 0;
    if (sc_memoryGet(ic, &cell_value) != 0) {
        sc_regSet(FLAG_MEMORY_OUT_OF_BOUNDS, 1);
        sc_regSet(FLAG_CLOCK_PULSES_IGNORED, 1);
        printFlags();
        fflush(stdout);
        return;
    }

    /* ── 2. Decode ───────────────────────────────────────────────────── */
    int sign = 0, command = 0, operand = 0;
    if (sc_commandDecode(cell_value, &sign, &command, &operand) != 0) {
        sc_regSet(FLAG_COMMAND_NOT_FOUND, 1);
        sc_regSet(FLAG_CLOCK_PULSES_IGNORED, 1);
        printFlags();
        fflush(stdout);
        return;
    }

    /* ── 3. Validate ─────────────────────────────────────────────────── */
    if (sc_commandValidate(command) != 0) {
        sc_regSet(FLAG_COMMAND_NOT_FOUND, 1);
        sc_regSet(FLAG_CLOCK_PULSES_IGNORED, 1);
        printFlags();
        fflush(stdout);
        return;
    }

    /* ── 4. Execute ──────────────────────────────────────────────────── */
    int next_ic = (ic + 1) % MEMORY_SIZE;

    int acc = 0;
    sc_accumulatorGet(&acc);

    int ovf = 0;
    sc_regGet(FLAG_OVERFLOW, &ovf);

    switch (command) {
    case 40: /* JUMP — unconditional */
        next_ic = operand;
        break;

    case 41: /* JNEG — jump if accumulator < 0 */
        if (acc < 0) next_ic = operand;
        break;

    case 42: /* JZ — jump if accumulator == 0 */
        if (acc == 0) next_ic = operand;
        break;

    case 43: /* HALT — stop clock */
        sc_regSet(FLAG_CLOCK_PULSES_IGNORED, 1);
        printCommand();
        printFlags();
        fflush(stdout);
        return;    /* do not advance IC */

    case 55: /* JNS — jump if accumulator > 0 */
        if (acc > 0) next_ic = operand;
        break;

    case 56: /* JC — jump if overflow flag set */
        if (ovf) next_ic = operand;
        break;

    case 57: /* JNC — jump if overflow flag clear */
        if (!ovf) next_ic = operand;
        break;

    case 58: /* JP — jump if accumulator is even */
        if (acc % 2 == 0) next_ic = operand;
        break;

    case 59: /* JNP — jump if accumulator is odd */
        if (acc % 2 != 0) next_ic = operand;
        break;

    default:
        /* All other commands: delegate to ALU */
        ALU(command, operand);
        break;
    }

    /* ── 5. Writeback ────────────────────────────────────────────────── */
    sc_icounterSet(next_ic);

    printCommand();
    printFlags();
    printAccumulator();
    printCounters();
    printBigChars();
    fflush(stdout);
}
