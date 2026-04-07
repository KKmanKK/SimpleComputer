#include "console.h"

int set_instruction_counter(int *selected_cell_index) {
    int ic;
    
    rk_mytermrestore();
    printf("Memory address: ");
    scanf("%d", &ic);
    rk_mytermregime(0, 0, 1, 0, 1);
    if (ic < 0 || ic > 99) return -1;

    *selected_cell_index = ic;
    return 0; 
}