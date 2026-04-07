#include "mySimpleComputer.h"

int sc_memory[MEMORY_SIZE];
int CPU_cache[50];
int CPU_cache_lines[5];
time_t access_cache_time[5];

int registr = 0;