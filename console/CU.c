#include "console.h"

void CU(void) {
    int ic = 0;
    sc_icounterGet(&ic);

    int cell_value = 0;
    if (sc_memoryGet(ic, &cell_value) != 0) {
        sc_regSet(FLAG_MEMORY_OUT_OF_BOUNDS, 1);
        sc_regSet(FLAG_CLOCK_PULSES_IGNORED, 1);
        printFlags();
        fflush(stdout);
        return;
    }

    int sign = 0, command = 0, operand = 0;
    if (sc_commandDecode(cell_value, &sign, &command, &operand) != 0) {
        sc_regSet(FLAG_COMMAND_NOT_FOUND, 1);
        sc_regSet(FLAG_CLOCK_PULSES_IGNORED, 1);
        printFlags();
        fflush(stdout);
        return;
    }

    if (sc_commandValidate(command) != 0) {
        sc_regSet(FLAG_COMMAND_NOT_FOUND, 1);
        sc_regSet(FLAG_CLOCK_PULSES_IGNORED, 1);
        printFlags();
        fflush(stdout);
        return;
    }

    int next_ic = (ic + 1) % MEMORY_SIZE;
    int acc = 0;
    sc_accumulatorGet(&acc);
    int ovf = 0;
    sc_regGet(FLAG_OVERFLOW, &ovf);

    switch (command) {
    case 40:
        next_ic = operand;
        break;
    case 41:
        if (acc < 0) next_ic = operand;
        break;
    case 42:
        if (acc == 0) next_ic = operand;
        break;
    case 43:
        sc_regSet(FLAG_CLOCK_PULSES_IGNORED, 1);
        printCommand();
        printFlags();
        fflush(stdout);
        return;
    case 55:
        if (acc > 0) next_ic = operand;
        break;
    case 56:
        if (ovf) next_ic = operand;
        break;
    case 57:
        if (!ovf) next_ic = operand;
        break;
    case 58:
        if (acc % 2 == 0) next_ic = operand;
        break;
    case 59:
        if (acc % 2 != 0) next_ic = operand;
        break;
    default:
        ALU(command, operand);
        break;
    }

    sc_icounterSet(next_ic);
    printCommand();
    printFlags();
    printAccumulator();
    printCounters();
    printBigChars();
    fflush(stdout);
}