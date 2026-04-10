#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include "myBigChars.h"

/*
 * font.c — generate the big-character font file.
 *
 * Encoding table (18 symbols, codes 0-17):
 *
 *  Code | Symbol     Code | Symbol     Code | Symbol
 *  -----+-------     -----+-------     -----+-------
 *    0  |  0           6  |  6          12  |  C
 *    1  |  1           7  |  7          13  |  D
 *    2  |  2           8  |  8          14  |  E
 *    3  |  3           9  |  9          15  |  F
 *    4  |  4          10  |  A          16  |  +
 *    5  |  5          11  |  B          17  |  1
 *
 * Each symbol is stored as two ints (8×8 bitmap, 2×32 bits).
 * File format: raw binary, 18 × 2 × sizeof(int) bytes.
 */

#define FONT_FILE  "font.bin"
#define FONT_SIZE  18

int main(void) {
    /* Font table in encoding order (codes 0-17) */
    int font[FONT_SIZE][2] = {
        { BIG_SYMBOL_0    },  /*  0 → '0' */
        { BIG_SYMBOL_1    },  /*  1 → '1' */
        { BIG_SYMBOL_2    },  /*  2 → '2' */
        { BIG_SYMBOL_3    },  /*  3 → '3' */
        { BIG_SYMBOL_4    },  /*  4 → '4' */
        { BIG_SYMBOL_5    },  /*  5 → '5' */
        { BIG_SYMBOL_6    },  /*  6 → '6' */
        { BIG_SYMBOL_7    },  /*  7 → '7' */
        { BIG_SYMBOL_8    },  /*  8 → '8' */
        { BIG_SYMBOL_9    },  /*  9 → '9' */
        { BIG_SYMBOL_A    },  /* 10 → 'A' */
        { BIG_SYMBOL_B    },  /* 11 → 'B' */
        { BIG_SYMBOL_C    },  /* 12 → 'C' */
        { BIG_SYMBOL_D    },  /* 13 → 'D' */
        { BIG_SYMBOL_E    },  /* 14 → 'E' */
        { BIG_SYMBOL_F    },  /* 15 → 'F' */
        { BIG_SYMBOL_PLUS },  /* 16 → '+' */
        { BIG_SYMBOL_1    },  /* 17 → '1' */
    };

    int fd = open(FONT_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("font: open");
        return 1;
    }

    if (bc_bigcharwrite(fd, (int *)font, FONT_SIZE) != 0) {
        fprintf(stderr, "font: write error\n");
        close(fd);
        return 1;
    }

    close(fd);
    printf("Font saved to %s  (%d symbols, %d bytes)\n",
           FONT_FILE, FONT_SIZE, (int)(FONT_SIZE * 2 * sizeof(int)));
    return 0;
}
