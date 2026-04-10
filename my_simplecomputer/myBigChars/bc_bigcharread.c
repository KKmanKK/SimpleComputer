#include "myBigChars.h"

/*
 * bc_bigcharread — read big-char bitmaps from a file descriptor.
 *
 * fd         — open file descriptor for reading
 * big        — buffer to store the bitmaps (must hold need_count symbols)
 * need_count — maximum number of symbols to read
 * count      — output: actual number of symbols read
 *
 * Returns 0 on success, -1 on error or invalid arguments.
 */
int bc_bigcharread(int fd, int *big, int need_count, int *count) {
    if (fd < 0 || need_count <= 0 || !count) return -1;

    int read_bytes = read(fd, big, need_count * sizeof(int) * 2);
    *count = read_bytes / (int)(sizeof(int) * 2);

    return (read_bytes >= 0) ? 0 : -1;
}
