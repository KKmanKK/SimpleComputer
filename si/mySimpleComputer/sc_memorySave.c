#include "mySimpleComputer.h"

int sc_memorySave(char *filename) {
    if (filename == NULL)
        return -1;
    FILE *file = fopen(filename, "wb");
    if (file == NULL)
        return -1;
    size_t written = fwrite(sc_memory, sizeof(int), MEMORY_SIZE, file);
    fclose(file);
    return (written == MEMORY_SIZE) ? 0 : -1;
}
