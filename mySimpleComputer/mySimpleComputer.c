#include "mySimpleComputer.h"

int sc_memory[MEMORY_SIZE];
int CPU_cache[50];
int CPU_cache_lines[5];
time_t access_cache_time[5];
int registr = 0;

int sc_memoryInit() {  
    for (int i = 0; i < MEMORY_SIZE; i++) sc_memory[i] = 0;
    return 0;
}

int sc_memorySet(int address, int value) {
    if (address < 0 || address >= MEMORY_SIZE) {
        registr = registr | (1 << (MEMORY_OUT_OF_BOUNDS - 1));
        return -1;
    }
    else {

        for (int i = 0; i < 5; i++) {
            if (CPU_cache_lines[i] == (address / 10 * 10)) {

                if (address < 10) {
                    CPU_cache[i * 10 + address] = value; 
                }
                else {
                    CPU_cache[i * 10 + (address % (address / 10 * 10 ))] = value; 
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
                    CPU_cache[i * 10 + address] = value;
                }
                else {
                    CPU_cache[i * 10 + (address % (address / 10 * 10 ))] = value; 
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
            CPU_cache[min * 10 + address] = value;
        }
        else {
            CPU_cache[min * 10 + (address % (address / 10 * 10 ))] = value;
        }
         
        access_cache_time[min] = time(NULL);

        return 0;

        //sc_memory[address] = value;
        return 0;
    }
}

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

int sc_memorySave(char* filename) {    
    FILE* file = fopen (filename, "wb");
    if (file != NULL) {
        fwrite(sc_memory, 1, sizeof(sc_memory), file);
        fclose(file);
        return 0;
    }
    else return -1;
}

int sc_memoryLoad(char* filename) {
    FILE* file = fopen (filename, "rb");
    if (file != NULL) {
        fread(sc_memory, 1, sizeof(sc_memory), file);
        fclose(file);
        return 0;
    }
    else return -1;
}

int sc_regInit(void) {
    registr = 0;
    return 0;
}

int sc_regSet(int registrNum, int value) {
    if (registrNum < 0 || registrNum > REGISTER_SIZE) {
        return -1;
    }
    else {
        if (value == 1)
            registr = registr | (1 << (registrNum - 1));
        else {
            if (value == 0)
                registr = registr & (~(1 << (registrNum - 1)));
            else return -1;
        }         
        return 0;
    }
}

int sc_regGet(int registrNum, int* value) {
    if (registrNum < 0 || registrNum > REGISTER_SIZE) {
        return -1;
    }
    else {
        *value = (registr >> (registrNum - 1)) & 0x1;
        return 0;
    }
}

int sc_commandEncode(int command, int operand, int* value) {
    if (command < 0 || command > 127 || operand < 0 || operand > 127)
        return -1;
    else {
        *value = (command << 7) | operand;
        return 0;
    }
}

int sc_commandDecode(int value, int* command, int* operand) {
    if (value < 0 || value > 131071) {
        registr = registr | (1 << (COMMAND_NOT_FOUND - 1));
        return -1;
    }
    else {

        *command = (value >> 8) & 0xFF;
        *operand = value & 0xFF;   

        return 0;
    }
}

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