#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>
#include "../include/mySimpleComputer.h"
#include "../include/myTerm.h"
#include "../include/myBigChars.h"
#include "../include/myReadkey.h"

/* ========== ЦВЕТА ========== */
#define C_BLACK   0
#define C_RED     1
#define C_GREEN   2
#define C_YELLOW  3
#define C_BLUE    4
#define C_MAGENTA 5
#define C_CYAN    6
#define C_WHITE   7
#define C_DEFAULT 9

/* ========== ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ ========== */
int selected_cell = 0;
int running = 1;
int edit_mode = 0;
char edit_buffer[10];
int edit_pos = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

/* Шрифт для больших символов */
static int font[16][2] = {0};

/* ========== ПРОТОТИПЫ ========== */
void draw_interface(void);
void draw_memory(void);
void draw_registers(void);
void draw_in_out(void);
void draw_big_cell(void);
void load_font(void);
void edit_cell(void);
int alu(int command, int operand);
void cu(void);
void *clock_generator(void *arg);
void irc(int signum);
void select_next_cell(void);

/* ========== ЗАГРУЗКА ШРИФТА ========== */
void load_font(void) {
    FILE *f = fopen("font.bin", "rb");
    if (f) {
        for (int i = 0; i < 16; i++) {
            fread(font[i], sizeof(int), 2, f);
        }
        fclose(f);
    } else {
        /* Создаем примитивный шрифт */
        int zero[2] = {0x7E818181, 0x81817E00};
        int one[2] = {0x183C3C18, 0x18181800};
        int two[2] = {0x7E060C18, 0x30607E00};
        int three[2] = {0x7E060C1E, 0x0C067E00};
        int four[2] = {0x6060607E, 0x7E181800};
        int five[2] = {0x7E607E7E, 0x0C067E00};
        int six[2] = {0x7E607E7E, 0x7E7E7E00};
        int seven[2] = {0x7E060C18, 0x30303000};
        int eight[2] = {0x7E7E7E7E, 0x7E7E7E00};
        int nine[2] = {0x7E7E7E7E, 0x0C067E00};
        
        for (int i = 0; i < 2; i++) {
            font[0][i] = zero[i];
            font[1][i] = one[i];
            font[2][i] = two[i];
            font[3][i] = three[i];
            font[4][i] = four[i];
            font[5][i] = five[i];
            font[6][i] = six[i];
            font[7][i] = seven[i];
            font[8][i] = eight[i];
            font[9][i] = nine[i];
        }
    }
}

/* ========== ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ ========== */
void select_next_cell(void) {
    if (selected_cell + 1 < MEMORY_SIZE)
        selected_cell++;
    else
        selected_cell = 0;
}

/* ========== АЛУ (Арифметико-логическое устройство) ========== */
int alu(int command, int operand) {
    int mem_val;
    
    if (operand < 0 || operand >= MEMORY_SIZE) {
        sc_regSet(REG_MEMORY_ERROR, 1);
        sc_regSet(REG_IGNORE_TICKS, 1);
        return -1;
    }
    
    sc_memoryGet(operand, &mem_val);
    
    switch(command) {
        /* ADD - сложение */
        case 30:
            if (sc_accumulator + mem_val <= 65535) {
                sc_accumulatorSet(sc_accumulator + mem_val);
            } else {
                sc_regSet(REG_OVERFLOW, 1);
            }
            break;
            
        /* SUB - вычитание */
        case 31:
            if (sc_accumulator - mem_val >= 0) {
                sc_accumulatorSet(sc_accumulator - mem_val);
            } else {
                sc_regSet(REG_OVERFLOW, 1);
            }
            break;
            
        /* DIVIDE - деление */
        case 32:
            if (mem_val == 0) {
                sc_regSet(REG_DIVISION_ZERO, 1);
                sc_regSet(REG_IGNORE_TICKS, 1);
                return -1;
            } else {
                sc_accumulatorSet(sc_accumulator / mem_val);
            }
            break;
            
        /* MUL - умножение */
        case 33:
            if (sc_accumulator * mem_val <= 65535) {
                sc_accumulatorSet(sc_accumulator * mem_val);
            } else {
                sc_regSet(REG_OVERFLOW, 1);
            }
            break;
            
        /* NOT - инверсия */
        case 51:
            sc_memorySet(operand, ~sc_accumulator);
            break;
            
        /* AND - логическое И */
        case 52:
            sc_accumulatorSet(mem_val & sc_accumulator);
            break;
            
        /* OR - логическое ИЛИ */
        case 53:
            sc_accumulatorSet(mem_val | sc_accumulator);
            break;
            
        /* XOR - исключающее ИЛИ */
        case 54:
            sc_accumulatorSet(mem_val ^ sc_accumulator);
            break;
            
        /* SHL - сдвиг влево */
        case 60:
            sc_accumulatorSet(1 << mem_val);
            break;
            
        /* SHR - сдвиг вправо */
        case 61:
            sc_accumulatorSet(1 >> mem_val);
            break;
            
        default:
            sc_regSet(REG_INVALID_COMMAND, 1);
            return -1;
    }
    return 0;
}

/* ========== CU (Управляющее устройство) ========== */
void cu(void) {
    pthread_mutex_lock(&mutex);
    
    int value, command, operand;
    int rows, cols;
    
    sc_memoryGet(selected_cell, &value);
    sc_commandDecode(value, &command, &operand);
    
    /* Проверка, является ли ячейка командой (старший бит = 1) */
    if ((value >> 15) & 1) {
        switch(command) {
            /* Арифметические и логические операции */
            case 30: case 31: case 32: case 33:
            case 51: case 52: case 53: case 54:
            case 60: case 61:
                alu(command, operand);
                select_next_cell();
                break;
                
            /* READ - ввод с терминала */
            case 10:
                if (operand >= 0 && operand < MEMORY_SIZE) {
                    int input_val;
                    mt_gotoXY(23, 2);
                    mt_setfgcolor(C_YELLOW);
                    printf("Enter value for cell %03d: ", operand);
                    mt_setdefaultcolor();
                    scanf("%d", &input_val);
                    sc_memorySet(operand, input_val);
                    select_next_cell();
                } else {
                    sc_regSet(REG_MEMORY_ERROR, 1);
                    sc_regSet(REG_IGNORE_TICKS, 1);
                }
                break;
                
            /* WRITE - вывод на терминал */
            case 11:
                if (operand >= 0 && operand < MEMORY_SIZE) {
                    int out_val;
                    sc_memoryGet(operand, &out_val);
                    mt_gotoXY(23, 2);
                    mt_setfgcolor(C_GREEN);
                    printf("Cell %03d value: %d", operand, out_val);
                    mt_setdefaultcolor();
                    usleep(1000000);
                    select_next_cell();
                } else {
                    sc_regSet(REG_MEMORY_ERROR, 1);
                    sc_regSet(REG_IGNORE_TICKS, 1);
                }
                break;
                
            /* LOAD - загрузка в аккумулятор */
            case 20:
                if (operand >= 0 && operand < MEMORY_SIZE) {
                    int load_val;
                    sc_memoryGet(operand, &load_val);
                    sc_accumulatorSet(load_val);
                    select_next_cell();
                } else {
                    sc_regSet(REG_MEMORY_ERROR, 1);
                    sc_regSet(REG_IGNORE_TICKS, 1);
                }
                break;
                
            /* STORE - выгрузка из аккумулятора */
            case 21:
                if (operand >= 0 && operand < MEMORY_SIZE) {
                    sc_memorySet(operand, sc_accumulator);
                    select_next_cell();
                } else {
                    sc_regSet(REG_MEMORY_ERROR, 1);
                    sc_regSet(REG_IGNORE_TICKS, 1);
                }
                break;
                
            /* JUMP - безусловный переход */
            case 40:
                if (operand >= 0 && operand < MEMORY_SIZE) {
                    selected_cell = operand;
                } else {
                    sc_regSet(REG_MEMORY_ERROR, 1);
                    sc_regSet(REG_IGNORE_TICKS, 1);
                }
                break;
                
            /* JNEG - переход если отрицательное */
            case 41:
                if (operand >= 0 && operand < MEMORY_SIZE) {
                    if (sc_accumulator < 0)
                        selected_cell = operand;
                    else
                        select_next_cell();
                } else {
                    sc_regSet(REG_MEMORY_ERROR, 1);
                    sc_regSet(REG_IGNORE_TICKS, 1);
                }
                break;
                
            /* JZ - переход если ноль */
            case 42:
                if (operand >= 0 && operand < MEMORY_SIZE) {
                    if (sc_accumulator == 0)
                        selected_cell = operand;
                    else
                        select_next_cell();
                } else {
                    sc_regSet(REG_MEMORY_ERROR, 1);
                    sc_regSet(REG_IGNORE_TICKS, 1);
                }
                break;
                
            /* HALT - останов */
            case 43:
                sc_regSet(REG_IGNORE_TICKS, 1);
                break;
                
            default:
                sc_regSet(REG_INVALID_COMMAND, 1);
                sc_regSet(REG_IGNORE_TICKS, 1);
                break;
        }
    } else {
        /* Если не команда - переходим к следующей ячейке */
        select_next_cell();
    }
    
    draw_interface();
    pthread_mutex_unlock(&mutex);
}

/* ========== РЕДАКТИРОВАНИЕ ЯЧЕЙКИ ========== */
void edit_cell(void) {
    int value, cmd, op;
    sc_memoryGet(selected_cell, &value);
    sc_commandDecode(value, &cmd, &op);
    
    mt_gotoXY(23, 2);
    mt_setfgcolor(C_YELLOW);
    printf("Enter new value (CMD:OP, e.g. 1E:0A): ");
    mt_setdefaultcolor();
    
    memset(edit_buffer, 0, sizeof(edit_buffer));
    edit_pos = 0;
    
    int key;
    while (1) {
        rk_readkey(&key);
        
        if (key == KEY_ENTER) {
            if (edit_pos >= 5) {
                int new_cmd, new_op;
                if (sscanf(edit_buffer, "%2X:%2X", &new_cmd, &new_op) == 2) {
                    int new_value;
                    sc_commandEncode(new_cmd, new_op, &new_value);
                    sc_memorySet(selected_cell, new_value);
                }
            }
            break;
        } else if (key == KEY_ESCAPE) {
            break;
        } else if (key >= '0' && key <= '9') {
            if (edit_pos < 7) {
                edit_buffer[edit_pos++] = key;
                mt_gotoXY(23 + edit_pos, 2);
                printf("%c", key);
            }
        } else if (key >= 'A' && key <= 'F') {
            if (edit_pos < 7) {
                edit_buffer[edit_pos++] = key;
                mt_gotoXY(23 + edit_pos, 2);
                printf("%c", key);
            }
        }
    }
    
    mt_gotoXY(23, 2);
    printf("                                        ");
    mt_setdefaultcolor();
}

/* ========== ОСНОВНАЯ ФУНКЦИЯ ========== */
int main(int argc __attribute__((unused)), char *argv[] __attribute__((unused))) {
    int rows, cols;
    
    if (!isatty(STDIN_FILENO)) {
        fprintf(stderr, "Error: stdin is not a terminal\n");
        return 1;
    }
    
    mt_getscreensize(&rows, &cols);
    if (rows < 25 || cols < 80) {
        fprintf(stderr, "Terminal too small. Need at least 80x25\n");
        return 1;
    }
    
    load_font();
    
    sc_memoryInit();
    sc_accumulatorInit();
    sc_icounterInit();
    sc_regInit();
    sc_cacheInit();
    
    /* Установка тестовых значений */
    int test_val;
    sc_commandEncode(0x1E, 0x0A, &test_val);
    sc_memorySet(0, test_val);  /* ADD 10 */
    sc_memorySet(10, 100);
    sc_accumulatorSet(50);
    sc_icounterSet(0);
    
    mt_clrscr();
    mt_setcursorvisible(1);
    
    rk_mytermsave();
    rk_mytermregime(0, 0, 1, 0, 1);
    
    signal(SIGALRM, irc);
    pthread_t clock_thread;
    pthread_create(&clock_thread, NULL, clock_generator, NULL);
    
    int key;
    while (running) {
        draw_interface();
        
        if (edit_mode) {
            edit_cell();
            edit_mode = 0;
        }
        
        rk_readkey(&key);
        
        switch (key) {
            case KEY_UP:
                if (selected_cell >= 10) selected_cell -= 10;
                break;
            case KEY_DOWN:
                if (selected_cell + 10 < MEMORY_SIZE) selected_cell += 10;
                break;
            case KEY_LEFT:
                if (selected_cell > 0) selected_cell--;
                break;
            case KEY_RIGHT:
                if (selected_cell + 1 < MEMORY_SIZE) selected_cell++;
                break;
            case KEY_ENTER:
                edit_mode = 1;
                break;
            case KEY_F5:
                mt_gotoXY(23, 2);
                mt_setfgcolor(C_YELLOW);
                printf("Enter accumulator value: ");
                mt_setdefaultcolor();
                int acc_val;
                scanf("%d", &acc_val);
                sc_accumulatorSet(acc_val);
                break;
            case KEY_F6:
                mt_gotoXY(23, 2);
                mt_setfgcolor(C_YELLOW);
                printf("Enter instruction counter: ");
                mt_setdefaultcolor();
                int ic_val;
                scanf("%d", &ic_val);
                if (ic_val >= 0 && ic_val < MEMORY_SIZE)
                    selected_cell = ic_val;
                break;
            case KEY_L:
                sc_memoryLoad("memory.bin");
                break;
            case KEY_S:
                sc_memorySave("memory.bin");
                break;
            case KEY_R:
                sc_regSet(REG_IGNORE_TICKS, 0);
                break;
            case KEY_T:
                cu();
                break;
            case KEY_I:
                sc_memoryInit();
                sc_accumulatorInit();
                sc_icounterInit();
                sc_regInit();
                sc_cacheInit();
                selected_cell = 0;
                break;
            case KEY_ESCAPE:
                running = 0;
                break;
        }
    }
    
    rk_mytermrestore();
    mt_clrscr();
    mt_setcursorvisible(1);
    return 0;
}

/* ========== ГЕНЕРАТОР ТАКТОВ ========== */
void *clock_generator(void *arg __attribute__((unused))) {
    struct timespec ts = {.tv_sec = 0, .tv_nsec = 500000000};
    while (1) {
        nanosleep(&ts, NULL);
        raise(SIGALRM);
    }
    return NULL;
}

/* ========== ОБРАБОТЧИК ПРЕРЫВАНИЙ ========== */
void irc(int signum __attribute__((unused))) {
    int ignore_ticks;
    sc_regGet(REG_IGNORE_TICKS, &ignore_ticks);
    if (!ignore_ticks) {
        cu();
    }
}

/* ========== ОТРИСОВКА ИНТЕРФЕЙСА ========== */
void draw_interface(void) {
    mt_clrscr();
    
    mt_setfgcolor(C_CYAN);
    bc_box(1, 1, 24, 78);
    mt_gotoXY(2, 30);
    printf("=== SIMPLE COMPUTER ===");
    mt_setdefaultcolor();
    
    draw_memory();
    draw_registers();
    draw_in_out();
    draw_big_cell();
    
    bc_box(22, 1, 3, 78);
    mt_gotoXY(23, 2);
    mt_setfgcolor(C_CYAN);
    printf("w/s/a/d - move, Enter - edit, F5-ACC, F6-IC, l-load, s-save, r-run, t-step, i-reset, ESC-exit");
    mt_setdefaultcolor();
    
    mt_gotoXY(1, 25);
    printf("Selected: %03d | ACC: %d | IC: %d", selected_cell, sc_accumulator, sc_icounter);
}

void draw_memory(void) {
    int value, cmd, op;
    
    bc_box(4, 2, 13, 48);
    mt_gotoXY(5, 20);
    printf("MEMORY");
    
    for (int i = 0; i < MEMORY_SIZE; i++) {
        int x = 4 + (i % 10) * 5;
        int y = 6 + i / 10;
        
        if (y > 16) break;
        
        mt_gotoXY(x, y);
        
        sc_memoryGet(i, &value);
        sc_commandDecode(value, &cmd, &op);
        
        if (i == selected_cell) {
            mt_setbgcolor(C_WHITE);
            mt_setfgcolor(C_BLACK);
        }
        
        printf("%02X:%02X", cmd, op);
        mt_setdefaultcolor();
    }
}

void draw_registers(void) {
    int acc, ic, flags;
    sc_accumulatorGet(&acc);
    sc_icounterGet(&ic);
    
    bc_box(4, 52, 8, 25);
    mt_gotoXY(5, 58);
    printf("REGISTERS");
    
    mt_gotoXY(52, 6);
    printf("ACC: 0x%04X", acc);
    mt_gotoXY(52, 7);
    printf("ACC: %5d", acc);
    mt_gotoXY(52, 8);
    printf("IC:  0x%02X", ic);
    mt_gotoXY(52, 9);
    printf("IC:  %3d", ic);
    
    mt_gotoXY(52, 10);
    printf("FLAGS: ");
    
    for (int i = 1; i <= 5; i++) {
        sc_regGet(i, &flags);
        if (flags) mt_setfgcolor(C_RED);
        switch (i) {
            case 1: printf("M"); break;
            case 2: printf("E"); break;
            case 3: printf("0"); break;
            case 4: printf("P"); break;
            case 5: printf("T"); break;
        }
        mt_setdefaultcolor();
        printf(" ");
    }
}

void draw_in_out(void) {
    int value, cmd, op;
    
    bc_box(13, 52, 8, 25);
    mt_gotoXY(14, 58);
    printf("IN-OUT");
    
    for (int i = 0; i < 5; i++) {
        sc_memoryGet(i, &value);
        sc_commandDecode(value, &cmd, &op);
        mt_gotoXY(52, 15 + i);
        printf("%d: %02X:%02X", i, cmd, op);
    }
}

void draw_big_cell(void) {
    int value, cmd, op;
    int hex_digits[4];
    
    sc_memoryGet(selected_cell, &value);
    sc_commandDecode(value, &cmd, &op);
    
    bc_box(18, 2, 4, 48);
    mt_gotoXY(19, 10);
    printf("SELECTED CELL: %03d  %02X:%02X", selected_cell, cmd, op);
    
    mt_gotoXY(20, 5);
    printf("CMD:");
    
    hex_digits[0] = (cmd >> 4) & 0x0F;
    hex_digits[1] = cmd & 0x0F;
    hex_digits[2] = (op >> 4) & 0x0F;
    hex_digits[3] = op & 0x0F;
    
    for (int i = 0; i < 2; i++) {
        int idx = hex_digits[i];
        if (idx >= 0 && idx < 16 && font[idx][0] != 0) {
            bc_printbigchar(font[idx], 8 + i * 9, 20, C_CYAN, C_BLACK);
        }
    }
    
    mt_gotoXY(25, 20);
    printf(":");
    
    for (int i = 0; i < 2; i++) {
        int idx = hex_digits[2 + i];
        if (idx >= 0 && idx < 16 && font[idx][0] != 0) {
            bc_printbigchar(font[idx], 35 + i * 9, 20, C_YELLOW, C_BLACK);
        }
    }
}