#include "mySimpleComputer.h"

int sc_memoryLoad(char *filename) {
    if (filename == NULL)
        return -1;
    FILE *file = fopen(filename, "rb");
    if (file == NULL)
        return -1;
    int tmp[MEMORY_SIZE];
    size_t read = fread(tmp, sizeof(int), MEMORY_SIZE, file);
    fclose(file);
    if (read != MEMORY_SIZE)
        return -1;
    for (int i = 0; i < MEMORY_SIZE; i++)
        sc_memory[i] = tmp[i];
    return 0;
}
