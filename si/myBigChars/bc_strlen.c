#include "myBigChars.h"

/*
 * bc_strlen — count the number of Unicode characters in a UTF-8 string.
 *
 * UTF-8 encoding rules:
 *   0xxxxxxx              — 1-byte sequence (U+0000 .. U+007F)
 *   110xxxxx 10xxxxxx     — 2-byte sequence (U+0080 .. U+07FF)
 *   1110xxxx 10xxxxxx×2   — 3-byte sequence (U+0800 .. U+FFFF)
 *   11110xxx 10xxxxxx×3   — 4-byte sequence (U+10000 .. U+10FFFF)
 *
 * Returns 0 if str is NULL or any byte sequence cannot be decoded.
 */
int bc_strlen(char *str) {
    if (!str) return 0;

    unsigned char *p = (unsigned char *)str;
    int count = 0;

    while (*p) {
        unsigned char c = *p;
        int bytes;

        if      (c < 0x80)            bytes = 1;  /* 0xxxxxxx */
        else if ((c & 0xE0) == 0xC0)  bytes = 2;  /* 110xxxxx */
        else if ((c & 0xF0) == 0xE0)  bytes = 3;  /* 1110xxxx */
        else if ((c & 0xF8) == 0xF0)  bytes = 4;  /* 11110xxx */
        else return 0;                             /* invalid start byte */

        /* Validate continuation bytes (must be 10xxxxxx) */
        for (int i = 1; i < bytes; i++) {
            if ((p[i] & 0xC0) != 0x80) return 0;
        }

        p += bytes;
        count++;
    }

    return count;
}
