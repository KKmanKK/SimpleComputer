#include "console.h"

void printCommand(void) {
    int ic;
    sc_icounterGet(&ic);

    int raw;
    sc_memoryGet(ic, &raw);

    int sign, command, operand;
    sc_commandDecode(raw, &sign, &command, &operand);

    mt_gotoXY(CMD_ROW, RP_COL);

    if (sign == 1) {
        int valid = sc_commandValidate(command);
        if (valid != 0)
            printf("! +%d : %-3d      ", command, operand);
        else
            printf("  +%d : %-3d      ", command, operand);
    } else {
        printf("   %-10d      ", raw);
    }
}
