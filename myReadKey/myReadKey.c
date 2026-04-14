#include "myReadKey.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

struct termios original_term;

/* Save current terminal parameters */
int rk_mytermsave(void) {
    if (tcgetattr(STDIN_FILENO, &original_term) != 0)
        return -1;
    return 0;
}

/* Restore previously saved terminal parameters */
int rk_mytermrestore(void) {
    if (tcsetattr(STDIN_FILENO, TCSANOW, &original_term) != 0)
        return -1;
    return 0;
}

/* Switch terminal between modes */
int rk_mytermregime(int regime, int vtime, int vmin, int echo, int sigint) {
    struct termios options;

    if (tcgetattr(STDIN_FILENO, &options) != 0)
        return -1;

    if (regime == 1) {
        options.c_lflag |= ICANON;
    } else if (regime == 0) {
        options.c_lflag &= ~ICANON;
        options.c_cc[VTIME] = (cc_t)vtime;
        options.c_cc[VMIN]  = (cc_t)vmin;
    } else {
        return -1;
    }

    if (echo == 1)
        options.c_lflag |= ECHO;
    else if (echo == 0)
        options.c_lflag &= ~ECHO;
    else
        return -1;

    if (sigint == 1)
        options.c_lflag |= ISIG;
    else if (sigint == 0)
        options.c_lflag &= ~ISIG;
    else
        return -1;

    if (tcsetattr(STDIN_FILENO, TCSANOW, &options) != 0)
        return -1;

    return 0;
}

/*
 * Key sequences read from a VT100/xterm terminal (non-canonical mode):
 *
 *  Single-byte keys:
 *    letters i,l,r,s,t  -> 1 byte  (ASCII)
 *    digits  0-9         -> 1 byte  (ASCII '0'..'9')
 *    ENTER               -> 1 byte  (0x0A = LF)
 *    ESC                 -> 1 byte  (0x1B), only when no more bytes follow
 *
 *  Arrow keys (3 bytes each):
 *    ESC [ A  ->  UP
 *    ESC [ B  ->  DOWN
 *    ESC [ C  ->  RIGHT
 *    ESC [ D  ->  LEFT
 *
 *  Function keys (5 bytes each):
 *    ESC [ 1 5 ~  ->  F5
 *    ESC [ 1 7 ~  ->  F6
 */
int rk_readkey(enum keys *key) {
    unsigned char buf[8];

    ssize_t n = read(STDIN_FILENO, buf, sizeof(buf));
    if (n <= 0)
        return -1;

    /* --- single-byte keys --- */
    if (n == 1) {
        switch (buf[0]) {
            case 'i':   *key = KEY_I;      return 0;
            case 'l':   *key = KEY_L;      return 0;
            case 'r':   *key = KEY_R;      return 0;
            case 's':   *key = KEY_S;      return 0;
            case 't':   *key = KEY_T;      return 0;
            case '0':   *key = KEY_0;      return 0;
            case '1':   *key = KEY_1;      return 0;
            case '2':   *key = KEY_2;      return 0;
            case '3':   *key = KEY_3;      return 0;
            case '4':   *key = KEY_4;      return 0;
            case '5':   *key = KEY_5;      return 0;
            case '6':   *key = KEY_6;      return 0;
            case '7':   *key = KEY_7;      return 0;
            case '8':   *key = KEY_8;      return 0;
            case '9':   *key = KEY_9;      return 0;
            case '\n':  *key = KEY_ENTER;  return 0;
            case 0x1B:  *key = KEY_ESCAPE; return 0;
            default:    return -1;
        }
    }

    /* --- escape sequences: must start with ESC '[' --- */
    if (n >= 3 && buf[0] == 0x1B && buf[1] == '[') {

        /* Arrow keys: ESC [ {A|B|C|D} */
        if (n == 3) {
            switch (buf[2]) {
                case 'A': *key = KEY_UP;    return 0;
                case 'B': *key = KEY_DOWN;  return 0;
                case 'C': *key = KEY_RIGHT; return 0;
                case 'D': *key = KEY_LEFT;  return 0;
                default:  return -1;
            }
        }

        /* Function keys: ESC [ 1 {5|7} ~ */
        if (n == 5 && buf[2] == '1' && buf[4] == '~') {
            switch (buf[3]) {
                case '5': *key = KEY_F5; return 0;
                case '7': *key = KEY_F6; return 0;
                default:  return -1;
            }
        }
    }

    return -1;
}

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