#include "myReadkey.h"

int rk_mytermsave(void) {
    tcgetattr(0, &original_term);  
    return 0;
}