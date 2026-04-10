#include "console.h"

/* Font loaded from file at startup (18 big-char bitmaps). */
int g_font[FONT_SIZE][2];

/* Index of the currently selected (edited) memory cell (0-127). */
int g_selected = 0;
