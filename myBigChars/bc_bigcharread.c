#include "myBigChars.h"

int bc_bigcharread(int fd, int *big, int need_count, int *count) {
    if (fd < 0 || need_count <= 0 || !count) return -1;

    int read_bytes = read(fd, big, need_count * sizeof(int) * 2);
    *count = read_bytes / (sizeof(int) * 2);

    if (read_bytes >= 0) return 0;
    else return -1;
}