#include <stdio.h>
#include <unistd.h>
#include "../include/myBigChars.h"
#include "../include/myTerm.h"

/* ========== bc_printA - вывод строки с псевдографикой ========== */
int bc_printA(char *str) {
    if (str == NULL) return -1;
    
    printf("\033(0%s\033(B", str);   
    return 0;
}

/* ========== bc_box - отрисовка рамки ========== */
int bc_box(int x1, int y1, int x2, int y2) {
    if (x1 < 0 || x2 < 0 || y1 < 0 || y2 < 0) return -1;
    
    /* Верхняя граница */
    printf("\033[%d;%dH\033(0l", x1, y1);
    for (int i = 1; i < y2; i++) {
        printf("q");
    }
    printf("k\n");
    
    /* Боковые границы */
    for (int i = 1; i < x2; i++) {
        printf("\033[%d;%dHx", x1 + i, y1);
        printf("\033[%d;%dHx", x1 + i, y1 + y2 - 1);
    }
    
    /* Нижняя граница */
    printf("\033[%d;%dHm", x1 + x2 - 1, y1);
    for (int i = 1; i < y2; i++) {
        printf("q");
    }
    printf("j\n\033(B");
    
    return 0;
}

/* ========== bc_printbigchar - вывод большого символа ========== */
int bc_printbigchar(int symbol[2], int x, int y, int fg_color, int bg_color) {
    if (fg_color < 0 || fg_color > 9 || bg_color < 0 || bg_color > 9) 
        return -1;
    if (x < 0 || y < 0) return -1;
    
    /* Приводим типы к enum colors из myTerm */
    enum colors fg = (enum colors)fg_color;
    enum colors bg = (enum colors)bg_color;
    
    /* Устанавливаем цвета */
    mt_setfgcolor(fg);
    mt_setbgcolor(bg);
    
    /* Переключаемся в режим псевдографики */
    printf("\033(0");
    
    int current_x = x;
    int current_y = y;
    
    /* Выводим символ построчно (8 строк) */
    for (int row = 0; row < 8; row++) {
        mt_gotoXY(current_x, current_y + row);
        
        /* Определяем из какого int брать данные (0 или 1) */
        int int_index = (row < 4) ? 0 : 1;
        int byte_index = row % 4;
        
        /* Получаем байт из int */
        int byte = (symbol[int_index] >> (byte_index * 8)) & 0xFF;
        
        /* Выводим 8 бит байта */
        for (int bit = 7; bit >= 0; bit--) {
            int pixel = (byte >> bit) & 1;
            if (pixel) {
                printf("a");  /* Закрашенный пиксель */
            } else {
                printf(" ");   /* Пустой пиксель */
            }
        }
    }
    
    /* Возвращаемся в обычный режим */
    printf("\033(B");
    mt_setdefaultcolor();
    
    return 0;
}

/* ========== bc_setbigcharpos - установка пикселя в большом символе ========== */
int bc_setbigcharpos(int *big, int x, int y, int value) {
    /* Приводим к 0-индексации */
    x--;
    y--;
    
    if (x < 0 || y < 0 || x > 7 || y > 7) return -1;
    if (value < 0 || value > 1) return -1;
    
    /* Определяем в каком int находится пиксель (0 или 1) */
    int index = (x < 4) ? 0 : 1;
    int row = (x < 4) ? x : x - 4;
    
    /* Позиция бита: байт = row, бит = 7 - y */
    int bit_pos = row * 8 + (7 - y);
    
    if (value == 1) {
        /* Устанавливаем бит в 1 */
        big[index] |= (1 << bit_pos);
    } else {
        /* Устанавливаем бит в 0 */
        big[index] &= ~(1 << bit_pos);
    }
    
    return 0;
}

/* ========== bc_getbigcharpos - получение пикселя из большого символа ========== */
int bc_getbigcharpos(int *big, int x, int y, int *value) {
    /* Приводим к 0-индексации */
    x--;
    y--;
    
    if (x < 0 || y < 0 || x > 7 || y > 7) return -1;
    if (value == NULL) return -1;
    
    /* Определяем в каком int находится пиксель (0 или 1) */
    int index = (x < 4) ? 0 : 1;
    int row = (x < 4) ? x : x - 4;
    
    /* Позиция бита: байт = row, бит = 7 - y */
    int bit_pos = row * 8 + (7 - y);
    
    *value = (big[index] >> bit_pos) & 1;
    
    return 0;
}

/* ========== bc_bigcharwrite - запись больших символов в файл ========== */
int bc_bigcharwrite(int fd, int *big, int count) {
    if (fd < 0 || count < 0) return -1;
    if (big == NULL) return -1;
    
    /* Каждый символ состоит из 2 int (8x8 = 64 бита = 2 int по 32 бита) */
    int bytes_to_write = count * 2 * sizeof(int);
    int result = write(fd, big, bytes_to_write);
    
    if (result == bytes_to_write) return 0;
    return -1;
}

/* ========== bc_bigcharread - чтение больших символов из файла ========== */
int bc_bigcharread(int fd, int *big, int need_count, int *count) {
    if (fd < 0 || need_count <= 0) return -1;
    if (big == NULL || count == NULL) return -1;
    
    int bytes_needed = need_count * 2 * sizeof(int);
    int bytes_read = read(fd, big, bytes_needed);
    
    if (bytes_read < 0) return -1;
    
    *count = bytes_read / (2 * sizeof(int));
    return 0;
}