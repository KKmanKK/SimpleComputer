#ifndef __GUARD_MYREADKEY_H
#define __GUARD_MYREADKEY_H

#define KEY_I 9 
#define KEY_L 12 
#define KEY_R 18 
#define KEY_S 19 
#define KEY_T 20
#define KEY_1 27 
#define KEY_2 28 
#define KEY_3 29 
#define KEY_4 30 
#define KEY_5 31
#define KEY_6 32
#define KEY_7 33
#define KEY_8 34 
#define KEY_9 35 
#define KEY_0 36
#define KEY_UP 37
#define KEY_DOWN 38
#define KEY_LEFT 39
#define KEY_RIGHT 40
#define KEY_F5 41
#define KEY_F6 42
#define KEY_ENTER 10
#define KEY_ESCAPE 27

#include <unistd.h>
#include <termios.h>

extern struct termios original_term;

int rk_readkey (int *key);

int rk_mytermsave(void);

int rk_mytermrestore(void);

int rk_mytermregime (int regime, int vtime, int vmin, int echo, int
sigint);

#endif /* __GUARD_MYREADKEY_H */