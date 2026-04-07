#include "myBigChars.h"

int bc_bigcharwrite(int fd, int *big, int count) {
    if (fd < 0 || count < 0) return -1;

    int result = write(fd, big, count * sizeof(int) * 2);
    if (result == count * sizeof(int) * 2) return 0;
    else return -1;
}