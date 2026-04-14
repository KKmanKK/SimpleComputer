#include "console.h"
#include <fcntl.h>
#include <errno.h>
#include <string.h>  

/* ГЛАВНАЯ ФУНКЦИЯ - ТОЧКА ВХОДА В ПРОГРАММУ */
int main(int argc, char *argv[]) {
    /* ===== 1. ЗАГРУЗКА ШРИФТА ===== */
    const char *font_file = (argc > 1) ? argv[1] : "font.bin";  /* имя файла шрифта */

    int fd = open(font_file, O_RDONLY);  /* открытие файла */
    if (fd < 0) {
        fprintf(stderr, "Error: cannot open font file '%s': %s\n",
                font_file, strerror(errno));
        return 1;
    }

    int font_count = 0;
    /* чтение шрифта из файла в глобальный массив g_font */
    if (bc_bigcharread(fd, (int *)g_font, FONT_SIZE, &font_count) != 0
            || font_count != FONT_SIZE) {
        fprintf(stderr,
                "Error: failed to read font from '%s' (read %d/%d symbols)\n",
                font_file, font_count, FONT_SIZE);
        close(fd);
        return 1;
    }
    close(fd);

    /* ===== 2. ПРОВЕРКА ТЕРМИНАЛА ===== */
    /* проверяем, что stdout является терминалом */
    if (!isatty(STDOUT_FILENO)) {
        fprintf(stderr, "Error: stdout is not a terminal\n");
        return 1;
    }

    /* проверяем размер терминала (нужно минимум 35x92) */
    int rows, cols;
    if (mt_getscreensize(&rows, &cols) != 0) {
        fprintf(stderr, "Error: cannot determine terminal size\n");
        return 1;
    }
    if (rows < MIN_ROWS || cols < MIN_COLS) {
        fprintf(stderr,
            "Error: terminal too small (%dx%d), need at least %dx%d\n",
            cols, rows, MIN_COLS, MIN_ROWS);
        return 1;
    }

    /* ===== 3. ИНИЦИАЛИЗАЦИЯ ЭМУЛЯТОРА ===== */
    sc_memoryInit();                      /* обнуление памяти */
    sc_accumulatorInit();                 /* аккумулятор = 0 */
    sc_icounterInit();                    /* счётчик команд = 0 */
    sc_regInit();                         /* все флаги = 0 */
    g_selected = 0;                       /* выбрана ячейка 0 */
    sc_regSet(FLAG_CLOCK_PULSES_IGNORED, 1);  /* T=1 (процессор остановлен) */

    /* ===== 4. ОТРИСОВКА ИНТЕРФЕЙСА ===== */
    mt_clrscr();                          /* очистка экрана */
    mt_setcursorvisible(0);               /* скрытие курсора */
    drawBoxes();                          /* рисование всех рамок */
    refreshAll();                         /* заполнение всех блоков данными */

    /* ===== 5. НАСТРОЙКА ТЕРМИНАЛА (RAW РЕЖИМ) ===== */
    rk_mytermsave();                      /* сохранение старых настроек */
    rk_mytermregime(0, 0, 1, 0, 1);       /* raw режим: без эха, без канонического ввода */

    /* ===== 6. РЕГИСТРАЦИЯ ОБРАБОТЧИКОВ СИГНАЛОВ ===== */
    signal(SIGALRM, IRC);                 /* таймер → IRC */
    signal(SIGUSR1, IRC);                 /* сброс (Reset) → IRC */
    signal(SIGUSR2, IRC);                 /* пошаговое выполнение → IRC */

    /* ===== 7. ЗАПУСК ТАЙМЕРА (каждые 0.5 секунды) ===== */
    struct itimerval timer;
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = 500000;      /* первый тик через 0.5 сек */
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 500000;   /* затем каждые 0.5 сек */
    setitimer(ITIMER_REAL, &timer, NULL);

    /* ===== 8. ГЛАВНЫЙ ЦИКЛ ОБРАБОТКИ КЛАВИШ ===== */
    char filename[256] = "memory.bin";    /* имя файла по умолчанию */

    while (1) {
        /* проверка: запущен ли процессор? */
        int running = 0;
        sc_regGet(FLAG_CLOCK_PULSES_IGNORED, &running);
        
        if (!running) {
            pause();                      /* процессор работает - ждём сигнал от таймера */
            continue;
        }

        /* процессор остановлен - читаем клавиши */
        int old = g_selected;
        enum keys key;
        if (rk_readkey(&key) != 0) continue;

        switch (key) {
            /* ----- СТРЕЛКИ - перемещение курсора по памяти ----- */
            case KEY_UP:
            case KEY_DOWN:
            case KEY_LEFT:
            case KEY_RIGHT:
                printCell(old, C_DEFAULT, C_DEFAULT);     /* снять выделение */
                moveCursor(key);                           /* переместить курсор */
                printCell(g_selected, C_BLACK, C_WHITE);   /* выделить новую ячейку */
                refreshSelected();                         /* обновить правую панель */
                break;

            /* ----- ENTER - редактирование ячейки ----- */
            case KEY_ENTER:
                mt_setcursorvisible(1);
                if (editCellInPlace(g_selected) == 0) {
                    printCell(g_selected, C_BLACK, C_WHITE);
                    refreshSelected();
                    printCommand();
                } else {
                    printCell(g_selected, C_BLACK, C_WHITE);
                }
                mt_setcursorvisible(0);
                fflush(stdout);
                break;

            /* ----- F5 - редактирование аккумулятора ----- */
            case KEY_F5:
                mt_setcursorvisible(1);
                if (editAccumulatorInPlace() == 0) {
                    printAccumulator();
                    printBigChars();
                }
                mt_setcursorvisible(0);
                fflush(stdout);
                break;

            /* ----- F6 - редактирование счётчика команд ----- */
            case KEY_F6:
                mt_setcursorvisible(1);
                if (editICInPlace() == 0) {
                    printCounters();
                    printCommand();
                }
                mt_setcursorvisible(0);
                fflush(stdout);
                break;

            /* ----- R - RUN (запуск программы) ----- */
            case KEY_R:
                sc_regSet(FLAG_CLOCK_PULSES_IGNORED, 0);   /* T=0 - процессор работает */
                printFlags();
                fflush(stdout);
                break;

            /* ----- S - STEP (пошаговое выполнение) ----- */
            case KEY_S:
                raise(SIGUSR2);                            /* принудительный шаг */
                break;

            /* ----- T - SAVE (сохранение памяти в файл) ----- */
            case KEY_T: {
                char buf[256];
                mt_setcursorvisible(1);
                if (promptLine("Save file: ", buf, sizeof(buf)) > 0)
                    strncpy(filename, buf, sizeof(filename) - 1);
                if (sc_memorySave(filename) != 0) {
                    promptLine("Error saving file. Press ENTER.", buf, sizeof(buf));
                }
                mt_setcursorvisible(0);
                break;
            }

            /* ----- L - LOAD (загрузка программы из файла) ----- */
            case KEY_L: {
                char buf[256];
                mt_setcursorvisible(1);
                if (promptLine("Load file: ", buf, sizeof(buf)) > 0)
                    strncpy(filename, buf, sizeof(filename) - 1);
                if (sc_memoryLoad(filename) == 0) {
                    refreshAll();                          /* полное обновление экрана */
                } else {
                    promptLine("Error loading file. Press ENTER.", buf, sizeof(buf));
                }
                mt_setcursorvisible(0);
                break;
            }

            /* ----- I - RESET (сброс) ----- */
            case KEY_I:
                raise(SIGUSR1);                            /* сигнал сброса */
                break;

            /* ----- ESC - выход из программы ----- */
            case KEY_ESCAPE:
                goto done;

            default:
                break;
        }
    }

/* ===== 9. ЗАВЕРШЕНИЕ РАБОТЫ ===== */
done:
    /* остановка таймера */
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 0;
    setitimer(ITIMER_REAL, &timer, NULL);

    /* восстановление терминала */
    rk_mytermrestore();
    mt_setcursorvisible(1);
    mt_gotoXY(MIN_ROWS + 1, 1);
    mt_setdefaultcolor();
    printf("\n");
    return 0;
}