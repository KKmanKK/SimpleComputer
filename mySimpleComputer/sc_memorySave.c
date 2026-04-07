#include "mySimpleComputer.h"

int sc_memorySave(char* filename) {    
    FILE* file = fopen (filename, "wb");
    if (file != NULL) {
        fwrite(sc_memory, 1, sizeof(sc_memory), file);
        fclose(file);
        return 0;
    }
    else return -1;
}