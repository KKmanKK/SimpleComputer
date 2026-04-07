#include "console.h"

int ALU(int command, int operand) {

    switch(command) {
        case 30: 
            if (operand >= 0 && operand < MEMORY_SIZE) {
                int memory_value;
                sc_memoryGet(operand, &memory_value);

                if (accumulator + memory_value <= 65535) {
                    accumulator += memory_value;
                }
                else {
                    sc_regSet(4, 1);
                    return 0; 
                }
            }
            else {
                sc_regSet(1, 1);
                sc_regSet(5, 1);
                return -1;
            }
            break;
        case 31: 
            if (operand >= 0 && operand < MEMORY_SIZE) {
                int memory_value;
                sc_memoryGet(operand, &memory_value);
                if (accumulator - memory_value >= 0) {
                    accumulator -= memory_value;
                }
                else {
                    sc_regSet(4, 1);
                    return 0; 
                }               
            }
            else {
                sc_regSet(1, 1);
                sc_regSet(5, 1);
                return -1;
            }
            break;
        case 32: 
            if (operand >= 0 && operand < MEMORY_SIZE) {
                int memory_value;
                sc_memoryGet(operand, &memory_value);

                if (memory_value == 0) {
                    sc_regSet(3, 1);
                    sc_regSet(5, 1);  
                    return -1;                  
                } else {
                    accumulator /= memory_value;
                }
            }
            else {
                sc_regSet(1, 1);
                sc_regSet(5, 1);
                return -1;
            }
            break;
        case 33: 
            if (operand >= 0 && operand < MEMORY_SIZE) {
                int memory_value;
                sc_memoryGet(operand, &memory_value);
                if (accumulator * memory_value <= 65535) {
                    accumulator *= memory_value;
                }
                else {
                    sc_regSet(4, 1);
                    return 0; 
                }
            }
            else {
                sc_regSet(1, 1);
                sc_regSet(5, 1);
                return -1;
            }
            break;
        case 51: 
            if (operand >= 0 && operand < MEMORY_SIZE) {
                    int value = ~accumulator;
                    sc_memorySet(operand, value);
                }
                else {
                    sc_regSet(1, 1);
                    sc_regSet(5, 1);
                    return -1;
                }
                break;
        case 52: 
            if (operand >= 0 && operand < MEMORY_SIZE) {
                    int memory_value;
                    sc_memoryGet(operand, &memory_value);

                    accumulator = memory_value & accumulator;
            }
            else {
                sc_regSet(1, 1);
                sc_regSet(5, 1);
                return -1;
            }
            break;
        case 53: 
            if (operand >= 0 && operand < MEMORY_SIZE) {
                int memory_value;
                sc_memoryGet(operand, &memory_value);

                accumulator = memory_value | accumulator;
            }
            else {
                sc_regSet(1, 1);
                sc_regSet(5, 1);
                return -1;
            }
            break;
        case 54: 
            if (operand >= 0 && operand < MEMORY_SIZE) {
                int memory_value;
                sc_memoryGet(operand, &memory_value);

                accumulator = memory_value ^ accumulator;
            }
            else {
                sc_regSet(1, 1);
                sc_regSet(5, 1);
                return -1;
            }
            break;
        case 60: 
            if (operand >= 0 && operand < MEMORY_SIZE) {
                int memory_value;
                sc_memoryGet(operand, &memory_value);

                accumulator = 1 << memory_value;
            }
            else {
                sc_regSet(1, 1);
                sc_regSet(5, 1);
                return -1;
            }
            break;
        case 61: 
            if (operand >= 0 && operand < MEMORY_SIZE) {
                int memory_value;
                sc_memoryGet(operand, &memory_value);

                accumulator = 1 >> memory_value;
            }
            else {
                sc_regSet(1, 1);
                sc_regSet(5, 1);
                return -1;
            }
            break;
        case 62: 
            if (operand >= 0 && operand < MEMORY_SIZE) {
                int memory_value;
                sc_memoryGet(operand, &memory_value);

                int bits = 16;

                memory_value %= bits;

                accumulator = (1 << memory_value) | (1 >> (bits - memory_value));
            }
            else {
                sc_regSet(1, 1);
                sc_regSet(5, 1);
                return -1;
            }
            break;
        case 63: 
            if (operand >= 0 && operand < MEMORY_SIZE) {
                int memory_value;
                sc_memoryGet(operand, &memory_value);

                int bits = 16;

                memory_value %= bits;

                accumulator = (1 >> memory_value) | (1 << (bits - memory_value));
            }
            else {
                sc_regSet(1, 1);
                sc_regSet(5, 1);
                return -1;
            }
            break;
        case 64: 
            if (operand >= 0 && operand < MEMORY_SIZE) {
                int memory_value;
                sc_memoryGet(operand, &memory_value);                

                accumulator = (~memory_value + 1);
            }
            else {
                sc_regSet(1, 1);
                sc_regSet(5, 1);
                return -1;
            }
            break;
        case 65: 
            if (operand >= 0 && operand < MEMORY_SIZE) {
                int first_value;
                int second_value;

                sc_memoryGet(operand, &first_value);

                if (((accumulator >> 16) & 1) == 0 && accumulator >= 0 && accumulator < MEMORY_SIZE) {
                    sc_memoryGet(accumulator, &second_value);    
                    accumulator = first_value + second_value;
                } 
                else {
                    sc_regSet(1, 1);
                    sc_regSet(5, 1);   
                    return -1;
                }                
            }
            else {
                sc_regSet(1, 1);
                sc_regSet(5, 1);
                return -1;
            }
            break;           
        case 66: 
            if (operand >= 0 && operand < MEMORY_SIZE) {
                int first_value;
                int second_value;

                sc_memoryGet(operand, &first_value);

                if (((accumulator >> 16) & 1) == 0 && accumulator >= 0 && accumulator < MEMORY_SIZE) {
                    sc_memoryGet(accumulator, &second_value);    
                    accumulator = first_value - second_value;
                } 
                else {
                    sc_regSet(1, 1);
                    sc_regSet(5, 1);   
                    return -1;
                }                
            }
            else {
                sc_regSet(1, 1);
            }
            break;
        case 67: 
            if (operand >= 0 && operand < MEMORY_SIZE) {
                
                int value;
                sc_memoryGet(operand, &value);

                if (((accumulator >> 16) & 1) == 0) {    
                    accumulator = value << accumulator;
                } 
                else {
                    sc_regSet(1, 1);
                    sc_regSet(5, 1);   
                    return -1;
                }                
            }
            else {
                sc_regSet(1, 1);
            }
            break;            
        case 68: 
                if (operand >= 0 && operand < MEMORY_SIZE) {
                
                int value;
                sc_memoryGet(operand, &value);

                if (((accumulator >> 16) & 1) == 0) {    
                    accumulator = value >> accumulator;
                } 
                else {
                    sc_regSet(1, 1);
                    sc_regSet(5, 1);   
                    return -1;
                }                
            }
            else {
                sc_regSet(1, 1);
                sc_regSet(5, 1);
                return -1;
            }
            break;            
        case 69: 
            if (operand >= 0 && operand < MEMORY_SIZE) {
                
                int value;
                sc_memoryGet(operand, &value);

                if (((accumulator >> 16) & 1) == 0) {    
                    value %= 16;
                    accumulator = (value << accumulator) | (value >> (16 - accumulator));
                } 
                else {
                    sc_regSet(1, 1);
                    sc_regSet(5, 1);   
                    return -1;
                }                
            }
            else {
                sc_regSet(1, 1);
                sc_regSet(5, 1);
                return -1;
            }
            break;            
        case 70: 
            if (operand >= 0 && operand < MEMORY_SIZE) {
                
                int value;
                sc_memoryGet(operand, &value);

                if (((accumulator >> 16) & 1) == 0) {    
                    value %= 16;
                    accumulator = (value >> accumulator) | (value << (16 - accumulator));
                } 
                else {
                    sc_regSet(1, 1);
                    sc_regSet(5, 1);   
                    return -1;
                }                
            }
            else {
                sc_regSet(1, 1);
                sc_regSet(5, 1);
                return -1;
            }
            break;                      
        case 71: 
            if (operand >= 0 && operand < MEMORY_SIZE) {
                
                int value;
                sc_memoryGet(operand, &value);

                if (((accumulator >> 16) & 1) == 0 && accumulator >= 0 && accumulator < MEMORY_SIZE) {    
                   
                   sc_memorySet(accumulator, value);
                } 
                else {
                    sc_regSet(1, 1);
                    sc_regSet(5, 1);   
                    return -1;
                }                
            }
            else {
                sc_regSet(1, 1);
                sc_regSet(5, 1);
                return -1;
            }
            break;         
        case 72: 
            if (operand >= 0 && operand < MEMORY_SIZE) {                             

                if (((accumulator >> 16) & 1) == 0 && accumulator >= 0 && accumulator < MEMORY_SIZE) {    
                   int value;
                   sc_memoryGet(accumulator, &value);

                   sc_memorySet(operand, value);
                } 
                else {
                    sc_regSet(1, 1);
                    sc_regSet(5, 1);   
                    return -1;
                }                
            }
            else {
                sc_regSet(1, 1);
                sc_regSet(5, 1);
                return -1;
            }
            break;        
        case 73: 
            if (operand >= 0 && operand < MEMORY_SIZE) {
                
                int value;
                sc_memoryGet(operand, &value);

                if (((accumulator >> 16) & 1) == 0 && accumulator >= 0 && accumulator < MEMORY_SIZE) {    
                    int addr;
                    sc_memoryGet(accumulator, &addr);

                    if (addr >= 0 && addr < MEMORY_SIZE && ((addr >> 16) & 1) == 0) {

                        sc_memorySet(addr, value);
                    }
                    else {
                        sc_regSet(1, 1);
                        sc_regSet(5, 1);   
                        return -1;
                    }
                } 
                else {
                    sc_regSet(1, 1);
                    sc_regSet(5, 1);   
                    return -1;
                }                
            }
            else {
                sc_regSet(1, 1);
                sc_regSet(5, 1);
                return -1;
            }
            break;    
        case 74: 
            if (operand >= 0 && operand < MEMORY_SIZE) {
                if (((accumulator >> 16) & 1) == 0 && accumulator >= 0 && accumulator < MEMORY_SIZE) {    
                    int first_addr;
                    sc_memoryGet(accumulator, &first_addr);

                    if (first_addr >= 0 && first_addr < MEMORY_SIZE && ((first_addr >> 16) & 1) == 0) {
                        int second_addr;
                        sc_memoryGet(accumulator, &second_addr);
                        if (second_addr >= 0 && second_addr < MEMORY_SIZE && ((second_addr >> 16) & 1) == 0) {
                            int value;
                            sc_memoryGet(second_addr, &value);
                            sc_memorySet(operand, value);
                        }
                        else {
                            sc_regSet(1, 1);
                            sc_regSet(5, 1);
                            return -1;
                        }                      
                    }
                    else {
                        sc_regSet(1, 1);
                        sc_regSet(5, 1);   
                        return -1;
                    }
                } 
                else {
                    sc_regSet(1, 1);
                    sc_regSet(5, 1);   
                    return -1;
                }                
            }
            else {
                sc_regSet(1, 1);
                sc_regSet(5, 1);
                return -1;
            }
            break;    
        case 75: 
            if (operand >= 0 && operand < MEMORY_SIZE) {
                if (((accumulator >> 16) & 1) == 0 && accumulator >= 0 && accumulator < MEMORY_SIZE) {    
                    int first_addr;
                    sc_memoryGet(accumulator, &first_addr);

                    if (first_addr >= 0 && first_addr < MEMORY_SIZE && ((first_addr >> 16) & 1) == 0) {
                        int second_addr;
                        sc_memoryGet(accumulator, &second_addr);
                        if (second_addr >= 0 && second_addr < MEMORY_SIZE && ((second_addr >> 16) & 1) == 0) {
                            int first_value;
                            int second_value;
                            sc_memoryGet(second_addr, &first_value);
                            sc_memoryGet(operand, &second_value);

                            accumulator = first_value + second_value;
                        }
                        else {
                            sc_regSet(1, 1);
                            sc_regSet(5, 1);   
                            return -1;
                        }                      
                    }
                    else {
                        sc_regSet(1, 1);
                        sc_regSet(5, 1);   
                        return -1;
                    }
                } 
                else {
                    sc_regSet(1, 1);
                    sc_regSet(5, 1);   
                    return -1;
                }                
            }
            else {
                sc_regSet(1, 1);
                sc_regSet(5, 1);
                return -1;
            }
            break;  
        case 76: 
            if (operand >= 0 && operand < MEMORY_SIZE) {
                if (((accumulator >> 16) & 1) == 0 && accumulator >= 0 && accumulator < MEMORY_SIZE) {    
                    int first_addr;
                    sc_memoryGet(accumulator, &first_addr);

                    if (first_addr >= 0 && first_addr < MEMORY_SIZE && ((first_addr >> 16) & 1) == 0) {
                        int second_addr;
                        sc_memoryGet(accumulator, &second_addr);
                        if (second_addr >= 0 && second_addr < MEMORY_SIZE && ((second_addr >> 16) & 1) == 0) {
                            int first_value;
                            int second_value;
                            sc_memoryGet(second_addr, &first_value);
                            sc_memoryGet(operand, &second_value);

                            accumulator = second_value - first_value;
                        }
                        else {
                            sc_regSet(1, 1);
                            sc_regSet(5, 1);   
                            return -1;
                        }                      
                    }
                    else {
                        sc_regSet(1, 1);
                        sc_regSet(5, 1);   
                        return -1;
                    }
                } 
                else {
                    sc_regSet(1, 1);
                    sc_regSet(5, 1);   
                    return -1;
                }                
            }
            else {
                sc_regSet(1, 1);
                sc_regSet(5, 1);
                return -1;
            }
            break;  
    }
    return 0;
}