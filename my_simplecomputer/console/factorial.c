#include <stdio.h>
#include <string.h>

/*
 * factorial.c — generates factorial.bin
 *
 * Reads n, computes n! and outputs the result.
 *
 * Memory layout:
 *   mem[20] = n      (input; decremented each iteration)
 *   mem[21] = result (accumulates the product)
 *   mem[99] = 1      (constant)
 *
 *   Addr  Instruction   Description
 *   00    READ  20      read n → mem[20]
 *   01    LOAD  20      acc = n
 *   02    STORE 21      result = n
 *   03    LOAD  20      acc = n             ← loop start
 *   04    SUB   99      acc = n - 1
 *   05    STORE 20      n = n - 1
 *   06    JZ    11      if n == 0 → done
 *   07    LOAD  21      acc = result
 *   08    MUL   20      acc = result * n
 *   09    STORE 21      result = acc
 *   10    JUMP   3      loop back
 *   11    WRITE 21      output result
 *   12    HALT
 *   99    =     1       constant
 *
 * Trace example (n=4):
 *   init:        mem[20]=4, mem[21]=4
 *   iter 1: n=3, result=4*3=12
 *   iter 2: n=2, result=12*2=24
 *   iter 3: n=1, result=24*1=24
 *   iter 4: n=0  → JZ → WRITE 24 → HALT
 *
 * Command codes:
 *   READ=10  WRITE=11  LOAD=20  STORE=21
 *   SUB=31   MUL=33    JUMP=40  JZ=42   HALT=43
 *
 * Encoding: (1 << 16) | (command << 8) | operand
 */

#define MEM(cmd, op)  ((1 << 16) | ((cmd) << 8) | (op))
#define MEMORY_SIZE   128

int main(void) {
    int mem[MEMORY_SIZE];
    memset(mem, 0, sizeof(mem));

    mem[ 0] = MEM(10, 20);  /* READ  20 — input n              */
    mem[ 1] = MEM(20, 20);  /* LOAD  20 — acc = n              */
    mem[ 2] = MEM(21, 21);  /* STORE 21 — result = n           */
    mem[ 3] = MEM(20, 20);  /* LOAD  20 — acc = n   (loop)     */
    mem[ 4] = MEM(31, 99);  /* SUB   99 — acc = n - 1          */
    mem[ 5] = MEM(21, 20);  /* STORE 20 — n = n - 1            */
    mem[ 6] = MEM(42, 11);  /* JZ    11 — if n == 0 → done     */
    mem[ 7] = MEM(20, 21);  /* LOAD  21 — acc = result         */
    mem[ 8] = MEM(33, 20);  /* MUL   20 — acc = result * n     */
    mem[ 9] = MEM(21, 21);  /* STORE 21 — result = acc         */
    mem[10] = MEM(40,  3);  /* JUMP   3 — loop back            */
    mem[11] = MEM(11, 21);  /* WRITE 21 — output result        */
    mem[12] = MEM(43,  0);  /* HALT                            */

    mem[99] = 1;             /* constant 1                      */

    FILE *f = fopen("factorial.bin", "wb");
    if (!f) { perror("fopen"); return 1; }
    fwrite(mem, sizeof(int), MEMORY_SIZE, f);
    fclose(f);

    printf("factorial.bin written (%d cells)\n", MEMORY_SIZE);
    return 0;
}
