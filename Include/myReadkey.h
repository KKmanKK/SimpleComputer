#ifndef __GUARD_MYREADKEY_H
#define __GUARD_MYREADKEY_H

#include <unistd.h>
#include <termios.h>

/* Enumeration of all recognized keys */
enum keys {
    KEY_0, KEY_1, KEY_2, KEY_3, KEY_4,
    KEY_5, KEY_6, KEY_7, KEY_8, KEY_9,
    KEY_I, KEY_L, KEY_R, KEY_S, KEY_T,
    KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT,
    KEY_F5, KEY_F6,
    KEY_ENTER,
    KEY_ESCAPE
};

extern struct termios original_term;

/* Read one key press and return its code via the pointer.
   Returns 0 on success, -1 if the key is not recognized. */
int rk_readkey(enum keys *key);

/* Save current terminal parameters. */
int rk_mytermsave(void);

/* Restore previously saved terminal parameters. */
int rk_mytermrestore(void);

/* Switch terminal between modes.
   regime : 1 = canonical, 0 = non-canonical.
   vtime  : VTIME value (tenths of a second, non-canonical only).
   vmin   : VMIN  value (min bytes before read returns, non-canonical only).
   echo   : 1 = enable ECHO, 0 = disable ECHO.
   sigint : 1 = enable ISIG, 0 = disable ISIG.
   Returns 0 on success, -1 on error. */
int rk_mytermregime(int regime, int vtime, int vmin, int echo, int sigint);

/* Read a signed hex value from the keyboard.
   Accepts format: [+-][0-9A-Fa-f]{1..4}, ended by ENTER.
   timeout : VTIME for the first character (tenths of a second);
             0 means wait indefinitely.
   The parsed integer is written to *value.
   Returns 0 on success, -1 on timeout or error. */
int rk_readvalue(int *value, int timeout);

#endif /* __GUARD_MYREADKEY_H */