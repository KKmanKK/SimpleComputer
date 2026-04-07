//
// Created by kchipson on 23.04.2020.
//
//
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "SimpleComputer.h"
#include "myUI.h"
#include "myReadkey.h"

void signalHandler(int signal);

int main() {
    ui_initial();
    signal(SIGALRM, signalHandler);
    signal(SIGUSR1, signalHandler);

    enum keys key;
    do {
        ui_update();
        rk_readKey(&key);
        switch(key) {
            case KEY_UP:    ui_moveCurrMemPointer(KEY_UP); break;
            case KEY_RIGHT: ui_moveCurrMemPointer(KEY_RIGHT); break;
            case KEY_DOWN:  ui_moveCurrMemPointer(KEY_DOWN); break;
            case KEY_LEFT:  ui_moveCurrMemPointer(KEY_LEFT); break;

            case KEY_L:     ui_loadMemory(); break;
            case KEY_S:     ui_saveMemory(); break;

            case KEY_R:     break;
            case KEY_T:     break;
            case KEY_I:     raise(SIGUSR1); break;

            case KEY_F5:    break;
            case KEY_F6:    ui_setICounter(); break;

            case KEY_ENTER: ui_setMCellValue(); break;
            
            default: break;
        }
    } while(key != KEY_ESC);

    return 0;
}

void signalHandler(int signal) {
    switch (signal) {
        case SIGALRM:
            break;
        case SIGUSR1:
            alarm(0);
            sc_regInit();
            sc_regSet(IGNORING_TACT_PULSES, true);
            sc_instructionCounter = 0;
            break;
        default:
            break;
    }
}