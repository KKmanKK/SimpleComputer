#include "console.h"

void CU() {    

    pthread_mutex_lock(&lock);
       
    int rows, cols;

    int value;
    int temp_selected_cell_index = selected_cell_index;
    sc_memoryGet(temp_selected_cell_index, &value);                    

    if (((value >> 16) & 1) == 1) {
    
        int command;
        int operand;

        sc_commandDecode(value, &command, &operand);

        switch(command) {

            case 30: case 31: case 32: case 33: case 51: 
            case 52: case 53: case 54: case 60: case 61: 
            case 62: case 63: case 64: case 65: case 66: 
            case 67: case 68: case 69: case 70: case 71: 
            case 72: case 73: case 74: case 75: case 76:
                int result = ALU(command, operand); 
                if (result == -1) sc_regSet(5, 1);
                select_next_cell();
                break;

            case 10: 
                if (operand < 0 || operand >= MEMORY_SIZE) {
                    sc_regSet(1, 1);
                    sc_regSet(5, 1); 
                    break;
                }
                else {                        
                    set_memory_cell(operand);
                    select_next_cell();
                    break;
                }
            case 11: 
                if (operand < 0 || operand >= MEMORY_SIZE) {
                    sc_regSet(1, 1);
                    sc_regSet(5, 1);
                    break; 
                }
                else {
                    int value;
                    sc_memoryGet(operand, &value);
                    printf("Cell %d value: %d\n", operand, value);                    
                    select_next_cell();
                    break;
                }
            case 20: 
                if (operand < 0 || operand >= MEMORY_SIZE) {
                    sc_regSet(1, 1);
                    sc_regSet(5, 1); 
                    break;
                }
                else {
                    sc_memoryGet(operand, &accumulator);           
                    select_next_cell();            
                    break;
                }
            case 21: 
                if (operand < 0 || operand >= MEMORY_SIZE) {
                    sc_regSet(1, 1);
                    sc_regSet(5, 1); 
                    break;
                }
                else {
                    sc_memorySet(operand, accumulator);    
                    select_next_cell();                   
                    break;
                }
            case 40: 
                if (operand < 0 || operand >= MEMORY_SIZE) {
                    sc_regSet(1, 1);
                    sc_regSet(5, 1); 
                    break;
                }
                else {
                    selected_cell_index = operand;                       
                    break;
                }                
            case 41: 
                if (operand < 0 || operand >= MEMORY_SIZE) {
                    sc_regSet(1, 1);
                    sc_regSet(5, 1);
                    break; 
                }
                else {
                    if (accumulator < 0) {
                        selected_cell_index = operand;  
                    }  
                    else {
                        select_next_cell();  
                    }                                           
                    break;
                }  
            case 42: 
                if (operand < 0 || operand >= MEMORY_SIZE) {
                    sc_regSet(1, 1);
                    sc_regSet(5, 1); 
                    break;
                }
                else {
                    if (accumulator == 0) {
                        selected_cell_index = operand;  
                    }       
                    else {
                        select_next_cell();  
                    }                                      
                    break;
                }  
            case 43: 
                sc_regSet(5, 1);                
                break;
            case 55: 
                if (operand < 0 || operand >= MEMORY_SIZE) {
                    sc_regSet(1, 1);
                    sc_regSet(5, 1);
                    break; 
                }
                else {
                    if (accumulator > 0) {
                        selected_cell_index = operand;  
                    }     
                    else {
                        select_next_cell();  
                    }                                           
                    break;
                }
            case 56: 
                if (operand < 0 || operand >= MEMORY_SIZE) {
                    sc_regSet(1, 1);
                    sc_regSet(5, 1);
                    break; 
                }
                else {
                    int reg;
                    sc_regGet(4, &reg);

                    if (reg) {
                        selected_cell_index = operand;  
                    }     
                    else {
                        select_next_cell();  
                    }                                           
                    break;
                }
                break;
            case 57: 
                if (operand < 0 || operand >= MEMORY_SIZE) {
                    sc_regSet(1, 1);
                    sc_regSet(5, 1);
                    break; 
                }
                else {
                    int reg;
                    sc_regGet(4, &reg);

                    if (!reg) {
                        selected_cell_index = operand;  
                    }     
                    else {
                        select_next_cell();  
                    }                                           
                    break;
                }
                break;
            case 58: 
                if (operand < 0 || operand >= MEMORY_SIZE) {
                    sc_regSet(1, 1);
                    sc_regSet(5, 1);
                    break; 
                }
                else {
                    if (accumulator % 2 == 0) {
                        selected_cell_index = operand;  
                    }     
                    else {
                        select_next_cell();  
                    }                                           
                    break;
                }
                break;
            case 59:                    
                                if (operand < 0 || operand >= MEMORY_SIZE) {
                    sc_regSet(1, 1);
                    sc_regSet(5, 1);
                    break; 
                }
                else {
                    if (accumulator % 2 != 0) {
                        selected_cell_index = operand;  
                    }     
                    else {
                        select_next_cell();  
                    }                                           
                    break;
                }
                break;
            default:
                sc_regSet(5, 1);
                sc_regSet(2, 1); 
                break;
        }   
        print_console(&rows, &cols);         
    }       
    else {
        select_next_cell();  
        print_console(&rows, &cols); 
    } 
    pthread_mutex_unlock(&lock);
}
