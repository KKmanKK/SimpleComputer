#include "console.h"
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>

/* Глобальные переменные */
int g_font[FONT_SIZE][2];     /* загруженный шрифт больших символов */
int g_selected = 0;           /* текущая выбранная ячейка памяти */

/* Кольцевой буфер для блока IN/OUT (хранит 4 последние записи) */
static struct {
    int address;    /* адрес ячейки */
    int input;      /* 1 - ввод, 0 - вывод */
    int value;      /* значение */
} history[INOUT_LINES];

static int history_count = 0;  /* количество записей в буфере */

/* ==================== ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ ==================== */

/* Количество ячеек в строке блока памяти (в последней строке может быть меньше) */
static int cells_in_row(int row) {
    int last = (row + 1) * MEM_CELLS_PER_ROW;
    return (last > MEMORY_SIZE) ? MEMORY_SIZE - row * MEM_CELLS_PER_ROW
                                 : MEM_CELLS_PER_ROW;
}

/* Получение индекса символа в шрифте (0-9, A-F) */
static int font_index(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return 10 + (c - 'A');
    return -1;
}

/* Вывод числа в двоичной системе (без ведущих нулей) */
static void print_binary(int value) {
    if (value == 0) {
        printf("0");
        return;
    }
    int bits = sizeof(int) * 8;
    int started = 0;
    for (int i = bits - 1; i >= 0; i--) {
        int bit = (value >> i) & 1;
        if (bit) started = 1;
        if (started) printf("%d", bit);
    }
}

/* ==================== РЕДАКТОР ПОЛЯ (IN-PLACE) ==================== */

/* Универсальный редактор поля
   row,col - позиция на экране
   width - ширина поля для очистки
   maxlen - максимальная длина ввода
   allowed - допустимые символы (NULL - любые печатные)
   out - буфер для введённой строки */
static int readField(int row, int col, int width, int maxlen,
                     const char *allowed, char *out) {
    /* Очистка поля */
    mt_gotoXY(row, col);
    for (int i = 0; i < width; i++)
        putchar(' ');
    mt_gotoXY(row, col);
    fflush(stdout);

    /* Переход в raw режим без эха */
    rk_mytermregime(0, 0, 1, 0, 0);

    int len = 0;
    int ret = -1;

    while (1) {
        unsigned char c;
        ssize_t n = read(STDIN_FILENO, &c, 1);
        if (n <= 0) break;

        if (c == '\n' || c == '\r') {  /* Enter - подтверждение */
            ret = 0;
            break;
        }

        if (c == 0x1B) {               /* ESC - отмена */
            /* Сброс возможных оставшихся байтов escape-последовательности */
            rk_mytermregime(0, 1, 0, 0, 0);
            unsigned char tmp[8];
            read(STDIN_FILENO, tmp, sizeof(tmp));
            rk_mytermregime(0, 0, 1, 0, 0);
            ret = -1;
            break;
        }

        if ((c == 127 || c == '\b') && len > 0) {  /* Backspace */
            len--;
            write(STDOUT_FILENO, "\b \b", 3);
            continue;
        }

        if (len >= maxlen) continue;

        /* Проверка допустимости символа */
        int ok;
        if (allowed == NULL) {
            ok = isprint((unsigned char)c);
        } else {
            ok = 0;
            for (const char *p = allowed; *p; p++) {
                if (*p == (char)c) { ok = 1; break; }
            }
        }
        if (!ok) continue;

        write(STDOUT_FILENO, &c, 1);  /* эхо-вывод */
        out[len++] = (char)c;
    }

    out[len] = '\0';
    rk_mytermregime(0, 0, 1, 0, 1);  /* возврат в raw режим с эхом */
    return ret;
}

/* ==================== ФУНКЦИЯ ВВОДА ИМЕНИ ФАЙЛА ==================== */

/* Запрос строки у пользователя (канонический режим) */
int promptLine(const char *label, char *buf, int bufsz) {
    rk_mytermregime(1, 0, 0, 1, 1);  /* канонический режим с эхом */
    mt_setcursorvisible(1);

    mt_gotoXY(PROMPT_ROW, 1);
    printf("%-*s", MIN_COLS, "");    /* очистка строки */
    mt_gotoXY(PROMPT_ROW, 1);
    printf("%s", label);
    fflush(stdout);

    buf[0] = '\0';
    if (fgets(buf, bufsz, stdin)) {
        int n = (int)strlen(buf);
        if (n > 0 && buf[n - 1] == '\n') buf[n - 1] = '\0';
    }

    mt_gotoXY(PROMPT_ROW, 1);
    printf("%-*s", MIN_COLS, "");    /* очистка строки */
    fflush(stdout);

    rk_mytermregime(0, 0, 1, 0, 1);  /* возврат в raw режим */
    mt_setcursorvisible(0);
    return (int)strlen(buf);
}

/* ==================== ФУНКЦИИ ОТРИСОВКИ ИНТЕРФЕЙСА ==================== */

/* Рисование всех рамок интерфейса */
void drawBoxes(void) {
    bc_box(BOX_MEM_ROW, BOX_MEM_COL, BOX_MEM_H, BOX_MEM_W,
           C_WHITE, C_DEFAULT, "Memory", C_YELLOW, C_DEFAULT);
    bc_box(BOX_INOUT_ROW, BOX_INOUT_COL, BOX_INOUT_H, BOX_INOUT_W,
           C_WHITE, C_DEFAULT, "IN/OUT", C_YELLOW, C_DEFAULT);
    bc_box(BOX_BIGCHAR_ROW, BOX_BIGCHAR_COL, BOX_BIGCHAR_H, BOX_BIGCHAR_W,
           C_WHITE, C_DEFAULT, "Accumulator", C_YELLOW, C_DEFAULT);
    bc_box(BOX_BIGCELL_ROW, BOX_BIGCELL_COL, BOX_BIGCELL_H, BOX_BIGCELL_W,
           C_WHITE, C_DEFAULT, "Cell", C_YELLOW, C_DEFAULT);
    bc_box(BOX_ACC_ROW, BOX_RP_COL, BOX_ACC_H, BOX_RP_W,
           C_WHITE, C_DEFAULT, "Accumulator", C_YELLOW, C_DEFAULT);
    bc_box(BOX_IC_ROW, BOX_RP_COL, BOX_IC_H, BOX_RP_W,
           C_WHITE, C_DEFAULT, "Instruction Counter", C_YELLOW, C_DEFAULT);
    bc_box(BOX_DECODED_ROW, BOX_RP_COL, BOX_DECODED_H, BOX_RP_W,
           C_WHITE, C_DEFAULT, "Selected Cell (format)", C_YELLOW, C_DEFAULT);
    bc_box(BOX_CMD_ROW, BOX_RP_COL, BOX_CMD_H, BOX_RP_W,
           C_WHITE, C_DEFAULT, "Command", C_YELLOW, C_DEFAULT);
    bc_box(BOX_FLAGS_ROW, BOX_RP_COL, BOX_FLAGS_H, BOX_RP_W,
           C_WHITE, C_DEFAULT, "Flags", C_YELLOW, C_DEFAULT);
    bc_box(BOX_KEYS_ROW, BOX_RP_COL, BOX_KEYS_H, BOX_RP_W,
           C_WHITE, C_DEFAULT, "Keys", C_YELLOW, C_DEFAULT);

    /* Подсказки по клавишам */
    mt_setfgcolor(C_WHITE);
    mt_gotoXY(KEYS_ROW1, RP_COL); printf("l-load  t-save  i-reset");
    mt_gotoXY(KEYS_ROW2, RP_COL); printf("r-run   s-step  ESC-quit");
    mt_gotoXY(KEYS_ROW3, RP_COL); printf("F5-acc  F6-ic");
    mt_setdefaultcolor();
}

/* Вывод ячейки памяти на экран в блоке "Memory" */
void printCell(int address, enum colors fg, enum colors bg) {
    int value;
    if (sc_memoryGet(address, &value) != 0) return;

    int row = MEM_ROW_ORIGIN + (address / MEM_CELLS_PER_ROW);
    int col = MEM_COL_ORIGIN + (address % MEM_CELLS_PER_ROW) * MEM_CELL_WIDTH;

    mt_gotoXY(row, col);
    mt_setfgcolor(fg);
    mt_setbgcolor(bg);

    int sign, command, operand;
    sc_commandDecode(value, &sign, &command, &operand);

    if (sign == 1)
        printf("+%04X ", value & 0xFFFF);   /* команда с плюсом */
    else
        printf(" %04X ", value & 0xFFFF);   /* данные */

    mt_setdefaultcolor();
}

/* Вывод регистра флагов (M E 0 P T) */
void printFlags(void) {
    static const char letters[FLAG_COUNT] = {'M', 'E', '0', 'P', 'T'};
    int value;

    mt_gotoXY(FLAGS_ROW, RP_COL);

    for (int i = 1; i <= FLAG_COUNT; i++) {
        sc_regGet(i, &value);
        printf("%c", value ? letters[i - 1] : '_');
        if (i < FLAG_COUNT) printf(" ");
    }
}

/* Вывод декодированной команды (DEC, OCT, HEX, BIN) */
void printDecodedCommand(int value) {
    mt_gotoXY(DECODED_ROW,     RP_COL); printf("DEC: %-10d",  value);
    mt_gotoXY(DECODED_ROW + 1, RP_COL); printf("OCT: %-10o",  value);
    mt_gotoXY(DECODED_ROW + 2, RP_COL); printf("HEX: %-10X",  value);
    mt_gotoXY(DECODED_ROW + 3, RP_COL); printf("BIN: ");
    print_binary(value);
    printf("          ");
}

/* Вывод аккумулятора (десятичный и шестнадцатеричный) */
void printAccumulator(void) {
    int value;
    sc_accumulatorGet(&value);
    mt_gotoXY(ACC_ROW,     RP_COL); printf("SC:  %-10d",  value);
    mt_gotoXY(ACC_ROW + 1, RP_COL); printf("HEX: %-10X",  (unsigned int)value);
}

/* Вывод счётчика команд */
void printCounters(void) {
    int value;
    sc_icounterGet(&value);
    mt_gotoXY(IC_ROW, RP_COL);
    printf("IC: %-4d", value);
}

/* Очистка блока IN/OUT */
void printTermClear(void) {
    history_count = 0;
    for (int i = 0; i < INOUT_LINES; i++) {
        mt_gotoXY(INOUT_ROW + i, INOUT_COL);
        printf("%-*s", BOX_INOUT_W - 2, "");
    }
    fflush(stdout);
}

/* Вывод строки в блок IN/OUT (с прокруткой) */
void printTerm(int address, int input) {
    int value = 0;
    sc_memoryGet(address, &value);

    /* Добавление записи в буфер */
    if (history_count < INOUT_LINES) {
        history[history_count].address = address;
        history[history_count].input   = input;
        history[history_count].value   = value;
        history_count++;
    } else {
        for (int i = 0; i < INOUT_LINES - 1; i++)
            history[i] = history[i + 1];
        history[INOUT_LINES - 1].address = address;
        history[INOUT_LINES - 1].input   = input;
        history[INOUT_LINES - 1].value   = value;
    }

    /* Перерисовка всех строк */
    for (int i = 0; i < history_count; i++) {
        mt_gotoXY(INOUT_ROW + i, INOUT_COL);
        if (history[i].input)
            printf("[%3d]: >              ", history[i].address);
        else
            printf("[%3d]: %-10d    ", history[i].address, history[i].value);
    }
}

/* Вывод текущей команды (по адресу из счётчика команд) */
void printCommand(void) {
    int ic;
    sc_icounterGet(&ic);

    int raw;
    sc_memoryGet(ic, &raw);

    int sign, command, operand;
    sc_commandDecode(raw, &sign, &command, &operand);

    mt_gotoXY(CMD_ROW, RP_COL);

    if (sign == 1) {
        int valid = sc_commandValidate(command);
        if (valid != 0)
            printf("! +%d : %-3d      ", command, operand);  /* неверная команда */
        else
            printf("  +%d : %-3d      ", command, operand);
    } else {
        printf("   %-10d      ", raw);
    }
}

/* Вывод больших символов аккумулятора (блок "Accumulator" большими цифрами) */
void printBigChars(void) {
    int value;
    sc_accumulatorGet(&value);

    /* Вывод знака "+" если старший бит установлен */
    if ((value >> 16) & 1)
        bc_printbigchar(g_font[16], BIGCHAR_ROW, 2, C_GREEN, C_DEFAULT);

    char digits[5];
    snprintf(digits, sizeof(digits), "%04X", value & 0xFFFF);

    /* Вывод цифр справа налево */
    int col = BIGCHAR_COL;
    for (int i = 3; i >= 0; i--) {
        int idx = font_index(digits[i]);
        if (idx >= 0)
            bc_printbigchar(g_font[idx], BIGCHAR_ROW, col, C_WHITE, C_DEFAULT);
        col -= 8;
    }
}

/* Вывод больших символов выбранной ячейки (блок "Cell" большими цифрами) */
void printBigCell(void) {
    int value = 0;
    sc_memoryGet(g_selected, &value);

    int sign, command, operand;
    sc_commandDecode(value, &sign, &command, &operand);

    if (sign == 1)
        bc_printbigchar(g_font[16], BIGCELL_ROW, BIGCELL_SIGN_COL,
                        C_GREEN, C_DEFAULT);

    char digits[5];
    snprintf(digits, sizeof(digits), "%04X", value & 0xFFFF);

    int col = BIGCELL_COL;
    for (int i = 3; i >= 0; i--) {
        int idx = font_index(digits[i]);
        if (idx >= 0)
            bc_printbigchar(g_font[idx], BIGCELL_ROW, col,
                            C_WHITE, C_DEFAULT);
        col -= 8;
    }
}

/* ==================== ПЕРЕМЕЩЕНИЕ КУРСОРА ==================== */

/* Перемещение выбранной ячейки в блоке памяти по стрелкам */
void moveCursor(enum keys dir) {
    int total_rows = (MEMORY_SIZE + MEM_CELLS_PER_ROW - 1) / MEM_CELLS_PER_ROW;
    int row = g_selected / MEM_CELLS_PER_ROW;
    int col = g_selected % MEM_CELLS_PER_ROW;

    switch (dir) {
        case KEY_LEFT:
            col = (col == 0) ? cells_in_row(row) - 1 : col - 1;
            break;
        case KEY_RIGHT:
            col = (col + 1) % cells_in_row(row);
            break;
        case KEY_UP: {
            int new_row = (row == 0) ? total_rows - 1 : row - 1;
            int cir = cells_in_row(new_row);
            col = (col < cir) ? col : cir - 1;
            row = new_row;
            break;
        }
        case KEY_DOWN: {
            int new_row = (row + 1) % total_rows;
            int cir = cells_in_row(new_row);
            col = (col < cir) ? col : cir - 1;
            row = new_row;
            break;
        }
        default:
            return;
    }
    g_selected = row * MEM_CELLS_PER_ROW + col;
}

/* ==================== IN-PLACE РЕДАКТОРЫ ==================== */

/* Редактирование ячейки памяти (ввод в шестнадцатеричном формате) */
int editCellInPlace(int address) {
    int row = MEM_ROW_ORIGIN + (address / MEM_CELLS_PER_ROW);
    int col = MEM_COL_ORIGIN + (address % MEM_CELLS_PER_ROW) * MEM_CELL_WIDTH;

    char buf[8];
    if (readField(row, col, MEM_CELL_WIDTH - 1, 5,
                  "+-0123456789ABCDEFabcdef", buf) != 0)
        return -1;

    if (buf[0] == '\0') return 0;

    int sign = 1;
    const char *hex = buf;
    if (buf[0] == '+') { sign = 1; hex = buf + 1; }
    else if (buf[0] == '-') { sign = -1; hex = buf + 1; }

    if (*hex == '\0') return 0;

    long raw = strtol(hex, NULL, 16);
    int val = (int)(sign < 0 ? -raw : raw);

    if (val < MEMORY_MIN_VALUE) val = MEMORY_MIN_VALUE;
    if (val > MEMORY_MAX_VALUE) val = MEMORY_MAX_VALUE;

    sc_memorySet(address, val);
    return 0;
}

/* Редактирование аккумулятора (ввод десятичного числа) */
int editAccumulatorInPlace(void) {
    char buf[12];
    if (readField(ACC_ROW, RP_COL + 5, 10, 6,
                  "+-0123456789", buf) != 0)
        return -1;

    if (buf[0] == '\0') return 0;

    int val = (int)strtol(buf, NULL, 10);
    if (val < ACC_MIN_VALUE) val = ACC_MIN_VALUE;
    if (val > ACC_MAX_VALUE) val = ACC_MAX_VALUE;

    sc_accumulatorSet(val);
    return 0;
}

/* Редактирование счётчика команд (ввод десятичного адреса) */
int editICInPlace(void) {
    char buf[6];
    if (readField(IC_ROW, RP_COL + 4, 6, 3,
                  "0123456789", buf) != 0)
        return -1;

    if (buf[0] == '\0') return 0;

    int val = (int)strtol(buf, NULL, 10);
    if (val < 0) val = 0;
    if (val >= MEMORY_SIZE) val = MEMORY_SIZE - 1;

    sc_icounterSet(val);
    return 0;
}

/* ==================== ОБНОВЛЕНИЕ ЭКРАНА ==================== */

/* Обновление панелей, зависящих от выбранной ячейки */
void refreshSelected(void) {
    int val = 0;
    sc_memoryGet(g_selected, &val);
    printDecodedCommand(val);
    printBigCell();
    fflush(stdout);
}

/* Полное обновление всего экрана */
void refreshAll(void) {
    printTermClear();
    for (int i = 0; i < MEMORY_SIZE; i++) {
        if (i == g_selected)
            printCell(i, C_BLACK, C_WHITE);   /* подсветка выбранной ячейки */
        else
            printCell(i, C_DEFAULT, C_DEFAULT);
    }
    printAccumulator();
    printCounters();
    printCommand();
    printFlags();
    printBigChars();
    refreshSelected();
    fflush(stdout);
}