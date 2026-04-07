#include "console.h"

int print_memory(int sc_memory[], int selected_cell_index) {
    if (sc_memory == NULL) return -1;
    int x = 2, y = 2;
    int value, selected_cell_value;
    for (int i = 0; i < 100; i+=10) {
        mt_gotoXY(x, y);
        for (int j = 0; j < 10; j++) {
            
            value = sc_memory[i + j];
            if (selected_cell_index == i + j) {
                mt_setfgcolor(BLACK);
                mt_setbgcolor(WHITE);
                if (((value >> 16) & 1) == 1) printf("+");
                else printf(" ");

                printf("%04X", (value & 0xFFFF));
                selected_cell_value = value;
                mt_setbgcolor(DEFAULT);
                mt_setfgcolor(DEFAULT);
                printf(" ");
            }
            else {
                if (((value >> 16) & 1) == 1) printf("+");
                else printf(" ");
                
                printf("%04X ", (value & 0xFFFF));
            }
        }
        printf("\n");
        y +=1; 
        x = 2;  
    }
    print_big_chars(selected_cell_value);
       
    mt_gotoXY (0, 31);
    return 0;
}