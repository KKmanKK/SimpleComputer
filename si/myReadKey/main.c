#include <stdio.h>
#include "myReadKey.h"

static const char *key_name(enum keys k) {
    switch (k) {
        case KEY_0: return "0"; case KEY_1: return "1";
        case KEY_2: return "2"; case KEY_3: return "3";
        case KEY_4: return "4"; case KEY_5: return "5";
        case KEY_6: return "6"; case KEY_7: return "7";
        case KEY_8: return "8"; case KEY_9: return "9";
        case KEY_I: return "i"; case KEY_L: return "l";
        case KEY_R: return "r"; case KEY_S: return "s";
        case KEY_T: return "t";
        case KEY_UP:     return "UP";
        case KEY_DOWN:   return "DOWN";
        case KEY_LEFT:   return "LEFT";
        case KEY_RIGHT:  return "RIGHT";
        case KEY_F5:     return "F5";
        case KEY_F6:     return "F6";
        case KEY_ENTER:  return "ENTER";
        case KEY_ESCAPE: return "ESC";
        default:         return "UNKNOWN";
    }
}

int main(void) {
    printf("=== rk_readkey test (press ESC twice to quit) ===\n");

    rk_mytermsave();
    /* Non-canonical, no echo, no signals, wait indefinitely */
    rk_mytermregime(0, 0, 1, 0, 0);

    int esc_count = 0;
    while (1) {
        enum keys key;
        if (rk_readkey(&key) == 0) {
            printf("Key: %s\n", key_name(key));
            fflush(stdout);
            if (key == KEY_ESCAPE && ++esc_count >= 2) break;
            else if (key != KEY_ESCAPE) esc_count = 0;
        }
    }

    rk_mytermrestore();

    /* --- rk_readvalue test --- */
    printf("\n=== rk_readvalue test ===\n");
    printf("Enter a hex value (e.g. +1A2B or -FF), timeout 50 ds: ");
    fflush(stdout);

    int val;
    if (rk_readvalue(&val, 50) == 0)
        printf("Parsed value: %d (0x%X)\n", val, (unsigned int)abs(val));
    else
        printf("Timeout or error.\n");

    return 0;
}
