#include "console.h"

int set_accumulator(int *accumulator) {
    int acc;
    
    rk_mytermrestore();
    printf("Accumulator value: ");
    while (1) {
        if (scanf("%d", &acc) == 0) {
            printf("Invalid value. Numbers only: ");
        }    
        else {
            if (acc < 0 || acc > 131071) {
                printf("Invalid value. Repeat input: ");        
            } 
            else break;
        }
        while (getchar() != '\n');
    }

    rk_mytermregime(0, 0, 1, 0, 1);
    *accumulator = acc;
    return 0; 
}