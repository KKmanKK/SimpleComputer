//
// Created by kchipson on 25.05.2020.
//

#ifndef MYREADKEY_H
#define MYREADKEY_H

#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

extern struct termios save;

enum keys {
    KEY_L,
    KEY_S,    // Должен быть определен для такта
    KEY_R,
    KEY_T,
    KEY_I,
    KEY_F5,
    KEY_F6,
    KEY_UP,
    KEY_DOWN,
    KEY_RIGHT,
    KEY_LEFT,
    KEY_ESC,
    KEY_ENTER,
    KEY_OTHER
};

int rk_pause(int time);
int rk_readKey(enum keys *key);
int rk_myTermSave(void);
int rk_myTermRestore(void);
int rk_myTermRegime(bool regime, unsigned int vtime, unsigned int vmin, bool echo, bool sigint);

#endif // MYREADKEY_H