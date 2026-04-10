#include "myReadKey.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*
 * rk_readvalue – interactive hex-value input from the terminal.
 *
 * Accepted format:  [+-][0-9A-Fa-f]{1,4}  followed by ENTER.
 *
 * The function switches the terminal to non-canonical mode so that each
 * character arrives immediately.  Echo is enabled so the user can see
 * what is being typed.  Backspace is supported.
 *
 * timeout – VTIME for the very first character (tenths of a second).
 *           0 means wait indefinitely.
 *
 * Returns:
 *   0   – success, *value contains the parsed integer (sign included).
 *  -1   – timeout waiting for first character, read error, or bad format.
 */
int rk_readvalue(int *value, int timeout) {
    if (value == NULL)
        return -1;

    /* Switch to non-canonical mode.
       timeout=0 means "wait indefinitely" → VMIN=1, VTIME=0 (blocking).
       timeout>0 means "wait up to timeout tenths" → VMIN=0, VTIME=timeout. */
    int vmin = (timeout == 0) ? 1 : 0;
    if (rk_mytermregime(0, timeout, vmin, 1, 0) != 0)
        return -1;

    char buf[8];   /* sign + up to 4 hex digits + '\0' */
    int  len = 0;
    int  ret = -1;

    while (1) {
        unsigned char c;
        ssize_t n = read(STDIN_FILENO, &c, 1);

        if (n <= 0) {
            /* Timeout or error */
            break;
        }

        /* After the first character arrived switch to blocking mode */
        if (len == 0) {
            rk_mytermregime(0, 0, 1, 1, 0);
        }

        if (c == '\n' || c == '\r') {
            /* ENTER – finish input */
            if (len > 0) ret = 0;
            write(STDOUT_FILENO, "\r\n", 2);
            break;
        }

        if ((c == 127 || c == '\b') && len > 0) {
            /* Backspace – erase last character */
            len--;
            write(STDOUT_FILENO, "\b \b", 3);
            continue;
        }

        /* First character must be sign or decimal digit */
        if (len == 0) {
            if (c != '+' && c != '-' && !isdigit((unsigned char)c))
                continue;
        } else {
            /* Subsequent characters: only decimal digits, max 5 chars total */
            if (!isdigit((unsigned char)c))
                continue;
            if (len >= 5)
                continue;
        }

        buf[len++] = (char)c;
    }

    rk_mytermregime(1, 0, 0, 1, 1); /* restore canonical mode */

    if (ret == 0) {
        buf[len] = '\0';

        int sign = 1;
        const char *hex_start = buf;

        if (buf[0] == '+') {
            sign = 1;
            hex_start = buf + 1;
        } else if (buf[0] == '-') {
            sign = -1;
            hex_start = buf + 1;
        }

        if (*hex_start == '\0') {
            ret = -1; /* only sign, no digits */
        } else {
            long parsed = strtol(hex_start, NULL, 10);
            *value = (int)(sign * parsed);
        }
    }

    return ret;
}
