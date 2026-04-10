#include "myBigChars.h"

/*
 * bc_printA — print a string using VT100 alternate charset (pseudo-graphics).
 *
 * str — the string to print; each character is mapped to the ACS symbol table.
 *
 * Returns 0 on success, -1 if str is NULL.
 */
int bc_printA(char *str) {
    if (str == NULL) return -1;

    printf("\033(0%s\033(B", str);
    return 0;
}
