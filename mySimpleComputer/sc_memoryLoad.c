#include "mySimpleComputer.h"

int sc_memoryLoad(char* filename) {
    FILE* file = fopen (filename, "rb");
    if (file != NULL) {
        fread(sc_memory, 1, sizeof(sc_memory), file);
        fclose(file);
        return 0;
    }
    else return -1;
}