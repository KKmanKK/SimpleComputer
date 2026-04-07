#include "console.h"

int print_flags(void) {
    int value;
    int y = 73;
    for (int i = 1; i <= REGISTER_SIZE; i++) {
        sc_regGet(i, &value);
        mt_gotoXY (y, 11); 
        char c;

        switch (i) {
            case 1:
                c = 'M';
                break;
            case 2:
                c = 'E';
                break;
            case 3:
                c = '0';
                break;  
            case 4:
                c = 'P';
                break;
            case 5:
                c = 'T';
                break;
            default: 
                return -1;
        }
        if (value) {
            printf("%c ", c);
            y += 2;
        }
    }
    mt_gotoXY (0, 31);
    return 0;
}