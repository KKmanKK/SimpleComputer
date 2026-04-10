#include "myBigChars.h"

/*
 * bc_bigcharwrite — write big-char bitmaps to a file descriptor.
 *
 * fd    — open file descriptor for writing
 * big   — array of two-int bitmaps (count symbols)
 * count — number of symbols to write
 *
 * Returns 0 on success, -1 on error or invalid arguments.
 */
int bc_bigcharwrite(int fd, int *big, int count) {
    if (fd < 0 || count < 0) return -1;

    int result = write(fd, big, count * sizeof(int) * 2);
    return (result == count * (int)(sizeof(int) * 2)) ? 0 : -1;
}
