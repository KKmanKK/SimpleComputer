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
    if (x1 < 1 || y1 < 1 || x2 < 1 || y2 < 1) return -1;
    
    /* Включаем режим псевдографики */
    printf("\033(0");
    
    /* Верхняя граница */
    mt_gotoXY(x1, y1);
    printf("l");  /* левый верхний угол */
    for (int i = 1; i < y2; i++) {
        printf("q");  /* горизонтальная линия */
    }
    printf("k");  /* правый верхний угол */
    
    /* Боковые границы и внутренность */
    for (int i = 1; i < x2; i++) {
        mt_gotoXY(x1 + i, y1);
        printf("x");  /* вертикальная линия */
        for (int j = 1; j < y2; j++) {
            printf(" ");
        }
        printf("x");
    }
    
    /* Нижняя граница */
    mt_gotoXY(x1 + x2 - 1, y1);
    printf("m");  /* левый нижний угол */
    for (int i = 1; i < y2; i++) {
        printf("q");  /* горизонтальная линия */
    }
    printf("j");  /* правый нижний угол */
    
    /* Выключаем режим псевдографики */
    printf("\033(B");
    
    return 0;
}

/* ========== bc_printbigchar - вывод большого символа ========== */
int bc_printbigchar(int symbol[2], int x, int y, int fg_color, int bg_color) {
    if (fg_color < 0 || fg_color > 9 || bg_color < 0 || bg_color > 9) 
        return -1;
    if (x < 0 || y < 0) return -1;
    
    /* Устанавливаем цвета */
    mt_setfgcolor(fg_color);
    mt_setbgcolor(bg_color);
    
    /* Выводим символ построчно (8 строк) */
    for (int row = 0; row < 8; row++) {
        mt_gotoXY(x, y + row);
        
        /* Определяем из какого int брать данные (0 или 1) */
        int int_index = (row < 4) ? 0 : 1;
        int byte_index = row % 4;
        
        /* Получаем байт из int (младший байт - верхняя строка) */
        int byte = (symbol[int_index] >> (byte_index * 8)) & 0xFF;
        
        /* Выводим 8 бит байта (слева направо) */
        for (int bit = 7; bit >= 0; bit--) {
            int pixel = (byte >> bit) & 1;
            if (pixel) {
                printf("@");  /* Закрашенный пиксель */
            } else {
                printf(" ");   /* Пустой пиксель */
            }
        }
    }
    
    /* Сбрасываем цвета */
    mt_setdefaultcolor();
    
    return 0;
}

/* ========== bc_setbigcharpos - установка пикселя ========== */
int bc_setbigcharpos(int *big, int x, int y, int value) {
    if (x < 1 || x > 8 || y < 1 || y > 8) return -1;
    if (value < 0 || value > 1) return -1;
    
    /* Приводим к 0-индексации */
    x--;
    y--;
    
    /* Определяем в каком int находится пиксель (0 или 1) */
    int index = (x < 4) ? 0 : 1;
    int row = (x < 4) ? x : x - 4;
    
    /* Позиция бита: байт = row, бит = 7 - y */
    int bit_pos = row * 8 + (7 - y);
    
    if (value == 1) {
        big[index] |= (1 << bit_pos);
    } else {
        big[index] &= ~(1 << bit_pos);
    }
    
    return 0;
}

/* ========== bc_getbigcharpos - получение пикселя ========== */
int bc_getbigcharpos(int *big, int x, int y, int *value) {
    if (x < 1 || x > 8 || y < 1 || y > 8) return -1;
    if (value == NULL) return -1;
    
    x--;
    y--;
    
    int index = (x < 4) ? 0 : 1;
    int row = (x < 4) ? x : x - 4;
    int bit_pos = row * 8 + (7 - y);
    
    *value = (big[index] >> bit_pos) & 1;
    
    return 0;
}

/* ========== bc_bigcharwrite - запись в файл ========== */
int bc_bigcharwrite(int fd, int *big, int count) {
    if (fd < 0 || count < 0) return -1;
    if (big == NULL) return -1;
    
    int bytes_to_write = count * 2 * sizeof(int);
    int result = write(fd, big, bytes_to_write);
    
    if (result == bytes_to_write) return 0;
    return -1;
}

/* ========== bc_bigcharread - чтение из файла ========== */
int bc_bigcharread(int fd, int *big, int need_count, int *count) {
    if (fd < 0 || need_count <= 0) return -1;
    if (big == NULL || count == NULL) return -1;
    
    int bytes_needed = need_count * 2 * sizeof(int);
    int bytes_read = read(fd, big, bytes_needed);
    
    if (bytes_read < 0) return -1;
    
    *count = bytes_read / (2 * sizeof(int));
    return 0;
}