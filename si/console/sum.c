#include <stdio.h>
#include <string.h>

/*
 * Generates sum.bin — a Simple Computer program that reads two numbers
 * and writes their sum.
 *
 * Program layout (MEMORY_SIZE = 128 ints):
 *
 *   Addr  Instruction     Description
 *   00    READ  10        read A from user → mem[10]
 *   01    READ  11        read B from user → mem[11]
 *   02    LOAD  10        acc = A
 *   03    ADD   11        acc = A + B
 *   04    STORE 12        mem[12] = acc
 *   05    WRITE 12        output mem[12]
 *   06    HALT            stop
 *
 * Command codes:
 *   READ  = 10,  WRITE = 11
 *   LOAD  = 20,  STORE = 21
 *   ADD   = 30
 *   HALT  = 43
 *
 * Encoding: (1 << 16) | (command << 8) | operand
 */

#define MEM(cmd, op)  ((1 << 16) | ((cmd) << 8) | (op))
#define MEMORY_SIZE   128

int main(void) {
    int mem[MEMORY_SIZE];
    memset(mem, 0, sizeof(mem));

    mem[0] = MEM(10, 10);  /* READ  10 — input A into mem[10] */
    mem[1] = MEM(10, 11);  /* READ  11 — input B into mem[11] */
    mem[2] = MEM(20, 10);  /* LOAD  10 — acc = A              */
    mem[3] = MEM(30, 11);  /* ADD   11 — acc = A + B          */
    mem[4] = MEM(21, 12);  /* STORE 12 — mem[12] = acc        */
    mem[5] = MEM(11, 12);  /* WRITE 12 — output result        */
    mem[6] = MEM(43,  0);  /* HALT                            */

    FILE *f = fopen("sum.bin", "wb");
    if (!f) { perror("fopen"); return 1; }
    fwrite(mem, sizeof(int), MEMORY_SIZE, f);
    fclose(f);

    printf("sum.bin written (%d cells)\n", MEMORY_SIZE);
    return 0;
}
