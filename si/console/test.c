#include <stdio.h>
#include "mySimpleComputer.h"
#include "console.h"

static int passed = 0;
static int failed = 0;

static void check(const char *name, int result, int expected) {
    if (result == expected) {
        printf("  [OK] %s\n", name);
        passed++;
    } else {
        printf("  [FAIL] %s: got %d, expected %d\n", name, result, expected);
        failed++;
    }
}

int main(void) {
    int val;

    /* --- sc_memoryInit --- */
    printf("=== sc_memoryInit ===\n");
    check("returns 0", sc_memoryInit(), 0);
    sc_memoryGet(0, &val);
    check("cell[0] == 0 after init", val, 0);
    sc_memoryGet(127, &val);
    check("cell[127] == 0 after init", val, 0);

    /* --- sc_memorySet --- */
    printf("=== sc_memorySet ===\n");
    check("valid data value",    sc_memorySet(0, 42), 0);
    check("valid command value", sc_memorySet(1, 131071), 0);
    check("address -1",         sc_memorySet(-1, 0), -1);
    check("address 128",        sc_memorySet(128, 0), -1);
    check("value negative",     sc_memorySet(0, -1), -1);
    check("value too large",    sc_memorySet(0, 131072), -1);

    /* --- sc_memoryGet --- */
    printf("=== sc_memoryGet ===\n");
    sc_memorySet(5, 1234);
    check("get valid cell",     sc_memoryGet(5, &val), 0);
    check("value matches",      val, 1234);
    check("address -1",         sc_memoryGet(-1, &val), -1);
    check("address 128",        sc_memoryGet(128, &val), -1);
    check("NULL pointer",       sc_memoryGet(0, NULL), -1);

    /* --- sc_memorySave / sc_memoryLoad --- */
    printf("=== sc_memorySave / sc_memoryLoad ===\n");
    sc_memoryInit();
    sc_memorySet(10, 100);
    sc_memorySet(20, 200);

    check("save returns 0",     sc_memorySave("test_memory.bin"), 0);
    check("save NULL filename", sc_memorySave(NULL), -1);

    sc_memoryInit();
    sc_memoryGet(10, &val);
    check("after init cell[10]==0", val, 0);

    check("load returns 0",     sc_memoryLoad("test_memory.bin"), 0);
    sc_memoryGet(10, &val);
    check("cell[10] restored",  val, 100);
    sc_memoryGet(20, &val);
    check("cell[20] restored",  val, 200);

    check("load NULL filename", sc_memoryLoad(NULL), -1);
    check("load bad file",      sc_memoryLoad("no_such_file.bin"), -1);

    /* load bad file must not corrupt memory */
    sc_memoryGet(10, &val);
    check("memory intact after bad load", val, 100);

    /* --- sc_regInit / sc_regSet / sc_regGet --- */
    printf("=== sc_regInit ===\n");
    check("returns 0", sc_regInit(), 0);
    sc_regGet(FLAG_MEMORY_OUT_OF_BOUNDS, &val);
    check("flag 1 == 0 after init", val, 0);

    printf("=== sc_regSet ===\n");
    check("set flag 1 to 1",      sc_regSet(FLAG_MEMORY_OUT_OF_BOUNDS, 1), 0);
    check("set flag 3 to 1",      sc_regSet(FLAG_DIVISION_BY_ZERO, 1), 0);
    check("set flag 1 to 0",      sc_regSet(FLAG_MEMORY_OUT_OF_BOUNDS, 0), 0);
    check("non-zero value = 1",   sc_regSet(FLAG_OVERFLOW, 42), 0);
    check("invalid flag 0",       sc_regSet(0, 1), -1);
    check("invalid flag 6",       sc_regSet(6, 1), -1);

    printf("=== sc_regGet ===\n");
    sc_regGet(FLAG_MEMORY_OUT_OF_BOUNDS, &val);
    check("flag 1 == 0 (was cleared)", val, 0);
    sc_regGet(FLAG_DIVISION_BY_ZERO, &val);
    check("flag 3 == 1",          val, 1);
    sc_regGet(FLAG_OVERFLOW, &val);
    check("flag 4 == 1 (non-zero set)", val, 1);
    check("invalid flag 0",       sc_regGet(0, &val), -1);
    check("NULL pointer",         sc_regGet(FLAG_MEMORY_OUT_OF_BOUNDS, NULL), -1);

    /* --- sc_accumulatorInit / Set / Get --- */
    printf("=== sc_accumulator ===\n");
    check("init returns 0",       sc_accumulatorInit(), 0);
    sc_accumulatorGet(&val);
    check("acc == 0 after init",  val, 0);
    check("set valid value",      sc_accumulatorSet(500), 0);
    sc_accumulatorGet(&val);
    check("acc == 500",           val, 500);
    check("set MIN_VALUE",        sc_accumulatorSet(-9999), 0);
    check("set MAX_VALUE",        sc_accumulatorSet(9999), 0);
    check("set too small",        sc_accumulatorSet(-10000), -1);
    check("set too large",        sc_accumulatorSet(10000), -1);
    sc_accumulatorGet(&val);
    check("acc unchanged after bad set", val, 9999);
    check("get NULL pointer",     sc_accumulatorGet(NULL), -1);

    /* --- sc_icounterInit / Set / Get --- */
    printf("=== sc_icounter ===\n");
    check("init returns 0",       sc_icounterInit(), 0);
    sc_icounterGet(&val);
    check("ic == 0 after init",   val, 0);
    check("set valid value",      sc_icounterSet(64), 0);
    sc_icounterGet(&val);
    check("ic == 64",             val, 64);
    check("set 0 (min)",          sc_icounterSet(0), 0);
    check("set 127 (max)",        sc_icounterSet(127), 0);
    check("set -1",               sc_icounterSet(-1), -1);
    check("set 128 (out of range)", sc_icounterSet(128), -1);
    sc_icounterGet(&val);
    check("ic unchanged after bad set", val, 127);
    check("get NULL pointer",     sc_icounterGet(NULL), -1);

    /* --- sc_commandEncode --- */
    printf("=== sc_commandEncode ===\n");
    int encoded;
    check("sign=1,cmd=10,op=5",   sc_commandEncode(1, 10, 5, &encoded), 0);
    check("encoded value correct", encoded, (1<<16)|(10<<8)|5);
    check("sign=0,cmd=0,op=0",    sc_commandEncode(0, 0, 0, &encoded), 0);
    check("encoded = 0",           encoded, 0);
    check("sign=1,cmd=255,op=255", sc_commandEncode(1, 255, 255, &encoded), 0);
    check("encoded = 131071",      encoded, 131071);
    check("bad sign 2",            sc_commandEncode(2, 10, 5, &encoded), -1);
    check("bad sign -1",           sc_commandEncode(-1, 10, 5, &encoded), -1);
    check("bad command 256",       sc_commandEncode(1, 256, 5, &encoded), -1);
    check("bad operand -1",        sc_commandEncode(1, 10, -1, &encoded), -1);
    check("NULL value ptr",        sc_commandEncode(1, 10, 5, NULL), -1);

    /* --- sc_commandDecode --- */
    printf("=== sc_commandDecode ===\n");
    int s, cmd, op;
    sc_commandEncode(1, 30, 99, &encoded);
    check("decode valid",          sc_commandDecode(encoded, &s, &cmd, &op), 0);
    check("sign == 1",             s, 1);
    check("command == 30",         cmd, 30);
    check("operand == 99",         op, 99);
    check("decode 0",              sc_commandDecode(0, &s, &cmd, &op), 0);
    check("sign == 0",             s, 0);
    check("decode 131071 (max)",   sc_commandDecode(131071, &s, &cmd, &op), 0);
    check("decode -1",             sc_commandDecode(-1, &s, &cmd, &op), -1);
    check("decode 131072",         sc_commandDecode(131072, &s, &cmd, &op), -1);
    check("NULL sign ptr",         sc_commandDecode(encoded, NULL, &cmd, &op), -1);
    check("NULL command ptr",      sc_commandDecode(encoded, &s, NULL, &op), -1);
    check("NULL operand ptr",      sc_commandDecode(encoded, &s, &cmd, NULL), -1);

    /* --- sc_commandValidate --- */
    printf("=== sc_commandValidate ===\n");
    check("READ (10)",             sc_commandValidate(10), 0);
    check("HALT (43)",             sc_commandValidate(43), 0);
    check("MOVA (71)",             sc_commandValidate(71), 0);
    check("invalid (0)",           sc_commandValidate(0), -1);
    check("invalid (15)",          sc_commandValidate(15), -1);
    check("invalid (255)",         sc_commandValidate(255), -1);

    /* --- print functions (visual output) --- */
    printf("=== printCell ===\n");
    sc_memoryInit();
    int enc;
    sc_commandEncode(1, 30, 5, &enc);
    sc_memorySet(0, enc);      /* command cell */
    sc_memorySet(1, 42);       /* data cell */
    printf("  command cell -> "); printCell(0,   C_DEFAULT, C_DEFAULT);
    printf("  data cell    -> "); printCell(1,   C_DEFAULT, C_DEFAULT);
    printf("  bad address  -> "); printCell(200, C_DEFAULT, C_DEFAULT);

    printf("=== printFlags ===\n");
    sc_regInit();
    printf("  all clear -> "); printFlags();
    sc_regSet(FLAG_MEMORY_OUT_OF_BOUNDS, 1);
    sc_regSet(FLAG_OVERFLOW, 1);
    printf("  M and P set -> "); printFlags();

    printf("=== printDecodedCommand ===\n");
    printf("  value=255:\n"); printDecodedCommand(255);
    printf("  value=0:\n");   printDecodedCommand(0);

    printf("=== printAccumulator ===\n");
    sc_accumulatorSet(1234);
    printf("  acc=1234 -> "); printAccumulator();

    printf("=== printCounters ===\n");
    sc_icounterSet(77);
    printf("  ic=77 -> "); printCounters();

    printf("\n=== Result: %d passed, %d failed ===\n", passed, failed);
    return (failed == 0) ? 0 : 1;
}
