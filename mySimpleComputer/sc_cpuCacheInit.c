#include "mySimpleComputer.h"

int sc_cpuCacheInit(void) {
    for (int i = 0; i < 50; i++) {
        CPU_cache[i] = 0;
    }

    for (int i = 0; i < 5; i++) {
        CPU_cache_lines[i] = -1;
        access_cache_time[i] = time(NULL);
    }

 
    return 0;
}