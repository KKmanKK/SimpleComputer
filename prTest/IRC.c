#include "console.h"

void IRC(int signum) {

    int clock_pulse_flag;
    sc_regGet(5, &clock_pulse_flag);

    if (clock_pulse_flag == 0) {  
        CU();
    }   
}