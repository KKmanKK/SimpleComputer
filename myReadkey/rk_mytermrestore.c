#include "myReadkey.h"

int rk_mytermrestore(void) {
    return tcsetattr(0, TCSANOW, &original_term); 
}