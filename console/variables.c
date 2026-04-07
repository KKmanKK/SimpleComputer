#include "console.h"

int accumulator = 0;

int instruction_counter = 0;

int selected_cell_index = 0;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;