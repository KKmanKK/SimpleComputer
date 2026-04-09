//
// Created by kchipson on 19.05.2020.
//

#ifndef MYUI_H
#define MYUI_H

#include <string.h>
#include "myBigChars.h"
#include "myReadkey.h"
#include "SimpleComputer.h"

int ui_initial(void);
int ui_update(void);
int ui_moveCurrMemPointer(enum keys key);
int ui_handleEnter(void);
int ui_handleF5(void);
int ui_handleF6(void);
int ui_saveMemory(void);
int ui_loadMemory(void);
int ui_reset(void);
int ui_start(void);
int ui_stop(void);
int ui_step(void);
int clearBuffIn(void);

#endif // MYUI_H