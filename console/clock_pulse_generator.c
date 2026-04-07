#include "console.h"
#include <time.h>

void* clock_pulse_generator(void* arg) {
    struct timespec ts = { .tv_sec = 0, .tv_nsec = 500000000 }; // 500 мс
    while (1) {
        nanosleep(&ts, NULL);
        raise(SIGALRM);
    }
    return NULL;
}