#include "myBigChars.h"

int bc_printA (char * str) {
    if (str == NULL) return -1;
    
    printf("\033(0%s\033(B", str);   
    return 0;
}