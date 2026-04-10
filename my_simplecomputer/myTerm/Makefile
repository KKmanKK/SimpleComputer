CC      = gcc
CFLAGS  = -g -Wall -I../include
AR      = ar
LIBRARY = libmyTerm.a
HEADER  = ../include/myTerm.h

SRCS = mt_clrscr.c mt_gotoXY.c mt_getscreensize.c \
       mt_setfgcolor.c mt_setbgcolor.c mt_setdefaultcolor.c \
       mt_setcursorvisible.c mt_delline.c
OBJS = $(SRCS:.c=.o)

.PHONY: all clean

all: $(LIBRARY) main

$(LIBRARY): $(OBJS)
	$(AR) rcs $(LIBRARY) $(OBJS)

main: main.o $(LIBRARY)
	$(CC) $(CFLAGS) main.o -L. -lmyTerm -o main

%.o: %.c $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -f $(LIBRARY) $(OBJS) main main.o
