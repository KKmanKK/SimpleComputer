#include "myReadKey.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

struct termios original_term;  /* сохранённые настройки терминала */

/* Сохранение текущих параметров терминала */
int rk_mytermsave(void) {
    if (tcgetattr(STDIN_FILENO, &original_term) != 0)
        return -1;
    return 0;
}

/* Восстановление ранее сохранённых параметров терминала */
int rk_mytermrestore(void) {
    if (tcsetattr(STDIN_FILENO, TCSANOW, &original_term) != 0)
        return -1;
    return 0;
}

/* Настройка режима работы терминала:
   regime: 1 - канонический, 0 - неканонический
   vtime, vmin - таймауты для неканонического режима
   echo: 1 - показывать вводимые символы, 0 - не показывать
   sigint: 1 - обрабатывать Ctrl+C, 0 - игнорировать */
int rk_mytermregime(int regime, int vtime, int vmin, int echo, int sigint) {
    struct termios options;

    if (tcgetattr(STDIN_FILENO, &options) != 0)
        return -1;

    /* Настройка канонического/неканонического режима */
    if (regime == 1) {
        options.c_lflag |= ICANON;      /* включить канонический режим */
    } else if (regime == 0) {
        options.c_lflag &= ~ICANON;     /* выключить канонический (raw режим) */
        options.c_cc[VTIME] = (cc_t)vtime;  /* таймаут */
        options.c_cc[VMIN]  = (cc_t)vmin;   /* минимальное количество символов */
    } else {
        return -1;
    }

    /* Настройка эха (отображения вводимых символов) */
    if (echo == 1)
        options.c_lflag |= ECHO;
    else if (echo == 0)
        options.c_lflag &= ~ECHO;
    else
        return -1;

    /* Настройка обработки сигналов (Ctrl+C, Ctrl+Z) */
    if (sigint == 1)
        options.c_lflag |= ISIG;
    else if (sigint == 0)
        options.c_lflag &= ~ISIG;
    else
        return -1;

    if (tcsetattr(STDIN_FILENO, TCSANOW, &options) != 0)
        return -1;

    return 0;
}

/* Чтение нажатой клавиши и определение её кода
   Поддерживает: буквы i,l,r,s,t, цифры 0-9, Enter, ESC,
   стрелки (UP/DOWN/LEFT/RIGHT), F5, F6 */
int rk_readkey(enum keys *key) {
    unsigned char buf[8];
    ssize_t n = read(STDIN_FILENO, buf, sizeof(buf));
    if (n <= 0)
        return -1;

    /* --- Однобайтовые клавиши --- */
    if (n == 1) {
        switch (buf[0]) {
            case 'i':   *key = KEY_I;      return 0;
            case 'l':   *key = KEY_L;      return 0;
            case 'r':   *key = KEY_R;      return 0;
            case 's':   *key = KEY_S;      return 0;
            case 't':   *key = KEY_T;      return 0;
            case '0':   *key = KEY_0;      return 0;
            case '1':   *key = KEY_1;      return 0;
            case '2':   *key = KEY_2;      return 0;
            case '3':   *key = KEY_3;      return 0;
            case '4':   *key = KEY_4;      return 0;
            case '5':   *key = KEY_5;      return 0;
            case '6':   *key = KEY_6;      return 0;
            case '7':   *key = KEY_7;      return 0;
            case '8':   *key = KEY_8;      return 0;
            case '9':   *key = KEY_9;      return 0;
            case '\n':  *key = KEY_ENTER;  return 0;
            case 0x1B:  *key = KEY_ESCAPE; return 0;
            default:    return -1;
        }
    }

    /* --- Escape-последовательности (начинаются с ESC [) --- */
    if (n >= 3 && buf[0] == 0x1B && buf[1] == '[') {

        /* Стрелки: ESC [ A/B/C/D (3 байта) */
        if (n == 3) {
            switch (buf[2]) {
                case 'A': *key = KEY_UP;    return 0;
                case 'B': *key = KEY_DOWN;  return 0;
                case 'C': *key = KEY_RIGHT; return 0;
                case 'D': *key = KEY_LEFT;  return 0;
                default:  return -1;
            }
        }

        /* Функциональные клавиши: ESC [ 1 5 ~ (F5) или ESC [ 1 7 ~ (F6) */
        if (n == 5 && buf[2] == '1' && buf[4] == '~') {
            switch (buf[3]) {
                case '5': *key = KEY_F5; return 0;
                case '7': *key = KEY_F6; return 0;
                default:  return -1;
            }
        }
    }

    return -1;
}

/* Интерактивный ввод числового значения с клавиатуры
   Формат: [+-][0-9]{1,5} (знак и до 5 цифр)
   timeout - время ожидания первого символа (в десятых долях секунды)
   Возвращает 0 при успехе, -1 при ошибке или таймауте */
int rk_readvalue(int *value, int timeout) {
    if (value == NULL)
        return -1;

    /* Переключение в неканонический режим для посимвольного ввода */
    int vmin = (timeout == 0) ? 1 : 0;
    if (rk_mytermregime(0, timeout, vmin, 1, 0) != 0)
        return -1;

    char buf[8];   /* буфер для вводимой строки */
    int  len = 0;
    int  ret = -1;

    while (1) {
        unsigned char c;
        ssize_t n = read(STDIN_FILENO, &c, 1);
        if (n <= 0)
            break;  /* таймаут или ошибка */

        /* После первого символа переключаемся в блокирующий режим */
        if (len == 0) {
            rk_mytermregime(0, 0, 1, 1, 0);
        }

        /* Enter - завершение ввода */
        if (c == '\n' || c == '\r') {
            if (len > 0) ret = 0;
            write(STDOUT_FILENO, "\r\n", 2);
            break;
        }

        /* Backspace - удаление последнего символа */
        if ((c == 127 || c == '\b') && len > 0) {
            len--;
            write(STDOUT_FILENO, "\b \b", 3);
            continue;
        }

        /* Проверка допустимых символов */
        if (len == 0) {
            /* Первый символ: +, - или цифра */
            if (c != '+' && c != '-' && !isdigit((unsigned char)c))
                continue;
        } else {
            /* Остальные символы: только цифры, не более 5 */
            if (!isdigit((unsigned char)c))
                continue;
            if (len >= 5)
                continue;
        }

        buf[len++] = (char)c;
    }

    /* Возврат в канонический режим */
    rk_mytermregime(1, 0, 0, 1, 1);

    /* Преобразование строки в число */
    if (ret == 0) {
        buf[len] = '\0';

        int sign = 1;
        const char *num_start = buf;

        if (buf[0] == '+') {
            sign = 1;
            num_start = buf + 1;
        } else if (buf[0] == '-') {
            sign = -1;
            num_start = buf + 1;
        }

        if (*num_start == '\0') {
            ret = -1;  /* только знак, нет цифр */
        } else {
            long parsed = strtol(num_start, NULL, 10);
            *value = (int)(sign * parsed);
        }
    }

    return ret;
}