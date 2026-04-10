#include "myReadKey.h"

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
