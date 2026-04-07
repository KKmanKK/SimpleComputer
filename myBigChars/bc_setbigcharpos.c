#include "myBigChars.h"

int bc_setbigcharpos (int * big, int x, int y, int value) {
    x--; y--;
    if (x < 0 || y < 0 || x > 7 || y > 7 || value < 0 || value > 1) return -1;    

    int index = (x < 4) ? 0 : 1;

    if (value == 1) 
        big[index] |= (1 << (y + (x % 4 * 8)));
    else 
        big[index] &= (~(1 << (y + (x % 4 * 8))));

    return 0;
}