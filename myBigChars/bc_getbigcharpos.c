#include "myBigChars.h"

int bc_getbigcharpos(int * big, int x, int y, int *value) {
    x--; y--;
    if (x < 0 || y < 0 || x > 7 || y > 7) return -1;  
    
    int index = (x < 4) ? 0 : 1;

    *value = big[index] >> (y + (x % 4 * 8)) & 0x1;

    return 0;
}
