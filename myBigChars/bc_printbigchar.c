#include "myBigChars.h"

int bc_printbigchar (int symbol[2], int x, int y, enum char_color fg_color, enum char_color bg_color) {
    if (fg_color < BLACK || fg_color > DEFAULT || bg_color < BLACK || fg_color > DEFAULT || x < 0 || y < 0) return -1;
    printf("\033[%d;%dH\033(0", x, y);

    for (int numbers = 0; numbers < 2; numbers++) {
        int temp_number = symbol[numbers];
        for (int i = 0; i < 4; i++) {
            int temp = temp_number & 0xff;
            for (int j = 0; j < 8; j++) {
                int rm_temp = temp & 1;
                if (rm_temp == 0) printf("\033[4%dm \033[49m", bg_color);
                else printf("\033[3%dm\033[4%dma\033[39m\033[49m", fg_color, bg_color);
                temp = temp >> 1;
            }
            printf("\n\033[%d;%dH", ++x, y);
            temp_number = temp_number >> 8; 
        }        
    }  
    printf("\033(B");
    printf("\033[%d;0H", x);

    return 0;
}