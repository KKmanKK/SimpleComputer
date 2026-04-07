#include "console.h"

int print_big_chars(int value) {
    
    int x = 14, y = 10;
    int big_symbol_0[2] = { BIG_SYMBOL_0 };
    int big_symbol_1[2] = { BIG_SYMBOL_1 };
    int big_symbol_2[2] = { BIG_SYMBOL_2 };
    int big_symbol_3[2] = { BIG_SYMBOL_3 };
    int big_symbol_4[2] = { BIG_SYMBOL_4 };
    int big_symbol_5[2] = { BIG_SYMBOL_5 };
    int big_symbol_6[2] = { BIG_SYMBOL_6 };
    int big_symbol_7[2] = { BIG_SYMBOL_7 };
    int big_symbol_8[2] = { BIG_SYMBOL_8 };
    int big_symbol_9[2] = { BIG_SYMBOL_9 };
    int big_symbol_plus[2] = { BIG_SYMBOL_PLUS };  

    int big_symbol_a[2] = { BIG_SYMBOL_A };
    int big_symbol_b[2] = { BIG_SYMBOL_B };
    int big_symbol_c[2] = { BIG_SYMBOL_C };
    int big_symbol_d[2] = { BIG_SYMBOL_D };
    int big_symbol_e[2] = { BIG_SYMBOL_E };
    int big_symbol_f[2] = { BIG_SYMBOL_F }; 

    if (((value >> 16) & 1) == 1) {
        bc_printbigchar(big_symbol_plus, x, 2, DEFAULT, DEFAULT);      
    }
    y = 34;

    char digits[5] = { '0', '0', '0', '0' };

    sprintf(digits, "%04X", value & 0xFFFF);
    
    for (int i = 3; i >=0; i--) {
        switch (digits[i]) {
            case ('0'): {           
                bc_printbigchar(big_symbol_0, x, y, DEFAULT, DEFAULT);                
                break; 
            };            
            case ('1'): { 
                bc_printbigchar(big_symbol_1, x, y, DEFAULT, DEFAULT); 
                break; 
            }
            case ('2'): { 
                bc_printbigchar(big_symbol_2, x, y, DEFAULT, DEFAULT); 
                break; 
            }  
            case ('3'): { 
                bc_printbigchar(big_symbol_3, x, y, DEFAULT, DEFAULT); 
                break; 
            }  
            case ('4'): { 
                bc_printbigchar(big_symbol_4, x, y, DEFAULT, DEFAULT); 
                break; 
            }  
            case ('5'): { 
                bc_printbigchar(big_symbol_5, x, y, DEFAULT, DEFAULT); 
                break; 
            }  
            case ('6'): { 
                bc_printbigchar(big_symbol_6, x, y, DEFAULT, DEFAULT); 
                break; 
            }  
            case ('7'): { 
                bc_printbigchar(big_symbol_7, x, y, DEFAULT, DEFAULT); 
                break; 
            }  
            case ('8'): { 
                bc_printbigchar(big_symbol_8, x, y, DEFAULT, DEFAULT); 
                break; 
            }  
            case ('9'): { 
                bc_printbigchar(big_symbol_9, x, y, DEFAULT, DEFAULT); 
                break; 
            }       
            case ('A'): {
                bc_printbigchar(big_symbol_a, x, y, DEFAULT, DEFAULT); 
                break;
            }
            case ('B'): {
                bc_printbigchar(big_symbol_b, x, y, DEFAULT, DEFAULT); 
                break;
            }
            case ('C'): {
                bc_printbigchar(big_symbol_c, x, y, DEFAULT, DEFAULT); 
                break;
            }
            case ('D'): {
                bc_printbigchar(big_symbol_d, x, y, DEFAULT, DEFAULT); 
                break;
            }
            case ('E'): {
                bc_printbigchar(big_symbol_e, x, y, DEFAULT, DEFAULT); 
                break;
            }
            case ('F'): {
                bc_printbigchar(big_symbol_f, x, y, DEFAULT, DEFAULT); 
                break;
            }
            default: {
                return -1;
            }
        }            
        y -= 8;     
    }
    return 0;
}