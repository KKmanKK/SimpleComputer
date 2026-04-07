#include "console.h"

int memory_cell_selection (int direction, int* selected_cell_index) {
    switch (direction) {
        case(KEY_UP):            
            if (0 <= *selected_cell_index - 10) *selected_cell_index -= 10;
            return 0;
        case(KEY_DOWN):
            if (*selected_cell_index + 10 < MEMORY_SIZE) *selected_cell_index += 10;
            return 0;
        case(KEY_LEFT):
            if (0 <= *selected_cell_index - 1) *selected_cell_index -= 1;
            return 0;  
        case(KEY_RIGHT):
            if (*selected_cell_index + 1 < MEMORY_SIZE) *selected_cell_index += 1;
            return 0;         
        default: 
            return -1;    
    }
}