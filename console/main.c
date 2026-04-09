//
// Created by kchipson on 23.04.2020.
//
#define _POSIX_C_SOURCE 199309L
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>

#include "SimpleComputer.h"
#include "myUI.h"
#include "myReadkey.h"

void signalHandler(int signal);
int running = 1;

// Функция для задержки в микросекундах
void delay_us(unsigned int microseconds) {
    struct timespec req;
    req.tv_sec = microseconds / 1000000;
    req.tv_nsec = (microseconds % 1000000) * 1000;
    nanosleep(&req, NULL);
}

int main() {
    ui_initial();
    signal(SIGALRM, IRC);      // Обработчик для тактов
    signal(SIGUSR1, IRC);      // Обработчик для сброса
    signal(SIGINT, IRC);
    enum keys key;
    while (running) {
        ui_update();
        
        // В режиме работы модели используем неблокирующее чтение
        if (sc_running) {
            // Неблокирующее чтение клавиши
            rk_myTermRegime(false, 0, 0, false, false);
            unsigned char ch = 0;  // unsigned char для корректного сравнения
            int bytes = read(STDIN_FILENO, &ch, 1);
            rk_myTermRestore();
            
            if (bytes > 0) {
                if (ch == 27) {  // ESC
                    sc_stop();
                    printf("\n[STOP] Model stopped by ESC\n");
                } else if (ch == 's' || ch == 'S') {
                    // Принудительный такт
                    kill(getpid(), SIGALRM);
                }
            }
            delay_us(10000); // Небольшая задержка для снижения нагрузки (10 мс)
        } else {
            // Интерактивный режим - блокирующее чтение
            rk_readKey(&key);
            
            switch(key) {
                case KEY_UP:    ui_moveCurrMemPointer(KEY_UP); break;
                case KEY_RIGHT: ui_moveCurrMemPointer(KEY_RIGHT); break;
                case KEY_DOWN:  ui_moveCurrMemPointer(KEY_DOWN); break;
                case KEY_LEFT:  ui_moveCurrMemPointer(KEY_LEFT); break;

                case KEY_L:     ui_loadMemory(); break;
                case KEY_S:     ui_saveMemory(); break;
                case KEY_I:     ui_reset(); break;
                case KEY_R:     sc_start(); break;      // Запуск модели
                case KEY_T:     ui_step(); break;       // Пошаговое выполнение

                case KEY_F5:    ui_handleF5(); break;
                case KEY_F6:    ui_handleF6(); break;

                case KEY_ENTER: ui_handleEnter(); break;
                case KEY_ESC:   running = 0; break;
                
                default: break;
            }
        }
    }

    // Восстановление терминала при выходе
    rk_myTermRestore();
    mt_clrScreen();
    return 0;
}

void signalHandler(int signal) {
    IRC(signal);
}