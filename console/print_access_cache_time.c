#include "console.h"

void print_access_cache_time() {
    // print time
    int y = 24;
    for (int i = 0; i < 5; i++) {
        if (CPU_cache_lines[i] != -1) {
            mt_gotoXY (85, y); 

            struct tm *current = localtime(&access_cache_time[i]);

            printf("%02d:%02d:%02d", current->tm_hour, current->tm_min, current->tm_sec);
            y++;
        }
    }
    mt_gotoXY (0, 29); 
}