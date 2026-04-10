#include "myReadKey.h"

int rk_mytermsave(void) {
    if (tcgetattr(STDIN_FILENO, &original_term) != 0)
        return -1;
    return 0;
}
