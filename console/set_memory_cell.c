#include "console.h"

int set_memory_cell(int address) {
    if (address < 0 || address >= MEMORY_SIZE) return -1;                  
    int cell_value;
    rk_mytermrestore();
    printf("Cell memory value: ");

    char buffer[256];

    while(1) {

        fgets(buffer, sizeof(buffer), stdin);

        if (sscanf(buffer, "%d", &cell_value) == 1) {
            if (cell_value < 0 || cell_value > 131071) {
            printf("Invalid value. Repeat input: ");
            }
            else break;           
        } 
        else {
            printf("Invalid value. Repeat input: ");
        }
        //scanf("%d", &cell_value);          
                    
    }
    rk_mytermregime(0, 0, 1, 0, 1);

    sc_memorySet(address, cell_value);
    return 0;
}