#include "console.h"

void print_CPU_cache() {
    mt_gotoXY (7, 24); 
    if (CPU_cache_lines[0] != -1) {
        for (int i = 0; i < 10; i++) {
            if (((CPU_cache[i] >> 16) & 1) == 1) printf("+");
            else printf(" ");
            printf("%04X ", (CPU_cache[i] & 0xFFFF));
        }
    }

    mt_gotoXY (7, 25); 
    if (CPU_cache_lines[1] != -1) {
        for (int i = 10; i < 20; i++) {
            if (((CPU_cache[i] >> 16) & 1) == 1) printf("+");
            else printf(" ");
            printf("%04X ", (CPU_cache[i] & 0xFFFF));
        }
    }
    mt_gotoXY (7, 26); 
    if (CPU_cache_lines[2] != -1) {
        for (int i = 20; i < 30; i++) {
            if (((CPU_cache[i] >> 16) & 1) == 1) printf("+");
            else printf(" ");
            printf("%04X ", (CPU_cache[i] & 0xFFFF));
        }
    }
    mt_gotoXY (7, 27); 
    if (CPU_cache_lines[3] != -1) {
        for (int i = 30; i < 40; i++) {
            if (((CPU_cache[i] >> 16) & 1) == 1) printf("+");
            else printf(" ");
            printf("%04X ", (CPU_cache[i] & 0xFFFF));
        }
    }
    mt_gotoXY (7, 28); 
    if (CPU_cache_lines[4] != -1) {
        for (int i = 40; i < 50; i++) {
            if (((CPU_cache[i] >> 16) & 1) == 1) printf("+");
            else printf(" ");
            printf("%04X ", (CPU_cache[i] & 0xFFFF));
        }
    }
    mt_gotoXY (0, 29);
}