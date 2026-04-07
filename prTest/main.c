//
// Created by kchipson on 23.04.2020.
//
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#include "SimpleComputer.h"
#include "myUI.h"
#include "myReadkey.h"

void signalHandler(int signal);
int running = 1;

int main() {
    ui_initial();
    signal(SIGALRM, IRC);      // Обработчик для тактов
    signal(SIGUSR1, IRC);      // Обработчик для сброса

    enum keys key;
    while (running) {
        ui_update();
        
        // В режиме работы модели используем неблокирующее чтение
        if (sc_running) {
            // Неблокирующее чтение клавиши
            rk_myTermRegime(false, 0, 0, false, false);
            int bytes = read(STDIN_FILENO, &key, sizeof(key));
            rk_myTermRestore();
            
            if (bytes > 0) {
                switch(key) {
                    case KEY_ESC:
                        sc_stop();
                        running = 0;
                        break;
                    case KEY_S:
                        // Принудительный такт
                        IRC(SIGALRM);
                        break;
                    default:
                        break;
                }
            }
            usleep(10000); // Небольшая задержка для снижения нагрузки
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