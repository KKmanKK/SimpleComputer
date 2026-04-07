#include "mySimpleComputer.h"

int sc_memoryGet(int address, int* value) {
    if (address < 0 || address >= MEMORY_SIZE) {
        registr = registr | (1 << (MEMORY_OUT_OF_BOUNDS - 1));
        return -1;
    }
    else {
    
        //check cell in cache
        for (int i = 0; i < 5; i++) {
            if (CPU_cache_lines[i] == (address / 10 * 10)) {

                if (address < 10) {
                    *value = CPU_cache[i * 10 + address]; 
                }
                else {
                    *value = CPU_cache[i * 10 + (address % (address / 10 * 10 ))]; 
                }
                  
                access_cache_time[i] = time(NULL);
                return 0;                            
            }
        }

        //load from memory to cache to first free line 
        for (int i = 0; i < 5; i++) {
            if (CPU_cache_lines[i] == -1) {

                sleep(3);
                
                CPU_cache_lines[i] = address / 10 * 10;
                for (int j = 0; j < 10; j++) {
                    CPU_cache[i * 10 + j] = sc_memory[address / 10 * 10 + j];
                }               

                if (address < 10) {
                    *value = CPU_cache[i * 10 + address];
                }
                else {
                    *value = CPU_cache[i * 10 + (address % (address / 10 * 10 ))]; 
                }

                access_cache_time[i] = time(NULL);
                return 0;
            }
        }

        //min access time
        time_t temp = access_cache_time[0];
        int min = 0;

        for (int i = 0; i < 5; i++)
        {
            if (access_cache_time[i] < temp) {
                temp = access_cache_time[i];
                min = i;
            }
        }

        //write from cache to memory
        sleep(3);
        for (int i = 0; i < 10; i++) {
           sc_memory[CPU_cache_lines[min] + i] = CPU_cache[min * 10 + i];
        }

        //load from memory to cache
        sleep(3);
        for (int i = 0; i < 10; i++) {
           CPU_cache[min * 10 + i] = sc_memory[address / 10 * 10 + i];
        }
        CPU_cache_lines[min] = address / 10 * 10;

        if (address < 10) {
            *value = CPU_cache[min * 10 + address];
        }
        else {
            *value = CPU_cache[min * 10 + (address % (address / 10 * 10 ))];
        }
         
        access_cache_time[min] = time(NULL);

        return 0;
        
        //sleep(5);
        //*value = sc_memory[address];
        return 0;        
        
    }    
}