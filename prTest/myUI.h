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
int ui_setMCellValue(void);
int ui_saveMemory(void);
int ui_loadMemory(void);
int ui_setICounter(void);

int clearBuffIn(void);

#endif // MYUI_H