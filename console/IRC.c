#include "console.h"

/* Контроллер прерываний (Interrupt Request Controller)
   Обрабатывает три типа сигналов от операционной системы:
   - SIGUSR1: сброс (Reset) - полная инициализация системы
   - SIGALRM: тактовый импульс - выполнение одной команды (если не остановлен)
   - SIGUSR2: принудительный шаг - выполнение одной команды (даже если остановлен) */
void IRC(int signum) {
    /* ===== SIGUSR1 - СБРОС (клавиша 'i') ===== */
    if (signum == SIGUSR1) {
        /* Сброс всех компонентов в начальное состояние */
        sc_memoryInit();                      /* очистка оперативной памяти */
        sc_accumulatorInit();                 /* аккумулятор = 0 */
        sc_icounterInit();                    /* счётчик команд = 0 */
        sc_regInit();                         /* все флаги = 0 */
        sc_regSet(FLAG_CLOCK_PULSES_IGNORED, 1);  /* T=1 (процессор остановлен) */
        
        g_selected = 0;                       /* курсор на ячейку 0 */
        
        /* Перерисовка интерфейса */
        mt_clrscr();                          /* очистка экрана */
        drawBoxes();                          /* отрисовка всех рамок */
        refreshAll();                         /* заполнение данными */
        return;
    }

    /* ===== SIGALRM - ТАКТОВЫЙ ИМПУЛЬС (от таймера, каждые 0.5 сек) ===== */
    if (signum == SIGALRM) {
        int flag = 0;
        sc_regGet(FLAG_CLOCK_PULSES_IGNORED, &flag);
        
        if (flag) return;  /* T=1 (HALT) - игнорируем такт */
        
        CU();              /* выполняем одну команду */
        return;
    }

    /* ===== SIGUSR2 - ПРИНУДИТЕЛЬНЫЙ ШАГ (клавиша 's') ===== */
    if (signum == SIGUSR2) {
        CU();              /* выполняем одну команду (даже если T=1) */
        return;
    }
}