#include "myReadKey.h"

int rk_mytermrestore(void) {
    if (tcsetattr(STDIN_FILENO, TCSANOW, &original_term) != 0)
        return -1;
    return 0;
}
