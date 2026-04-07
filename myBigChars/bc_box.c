#include "myBigChars.h"

int bc_box(int x1, int y1, int x2, int y2) {
    if (x1 < 0 || x2 < 0 || y1 < 0 || y2 < 0) return -1;
    
    printf("\033[%d;%dH\033(0l", x1, y1);

    for (int i = 1; i < y2; i++) {
        printf("q");
    }
    printf("k\n\033[%d;%dH", x1 + 1, y1);

    for (int i = 1; i < x2; i++) {
        printf("x");
        for (int j = 1; j < y2; j++) {
            printf(" ");
        }
        printf("x\n\033[%d;%dH", x1 + i, y1); 
    }

    printf("m");

    for (int i = 1; i < y2; i++) {
        printf("q");
    }
    printf("j\n\033(B");

    return 0;
}