CC      = gcc
CFLAGS  = -g -Wall -I../include

SRCS = bc_bigcharread.c bc_bigcharwrite.c bc_box.c \
       bc_getbigcharpos.c bc_printA.c bc_printbigchar.c \
       bc_setbigcharpos.c bc_strlen.c

OBJS    = $(SRCS:.c=.o)
LIBRARY = libmyBigChars.a
HEADER  = ../include/myBigChars.h

.PHONY: all clean

all: $(LIBRARY)

$(LIBRARY): $(OBJS)
	ar rcs $(LIBRARY) $(OBJS)

# Every object file depends on its own source AND on the shared header.
# This ensures the library is fully rebuilt whenever the header changes.
%.o: %.c $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(LIBRARY)
