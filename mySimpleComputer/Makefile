CC     = gcc
CFLAGS = -g -Wall -I../include

HEADER = ../include/mySimpleComputer.h

SRCS = mySimpleComputer.c

OBJS = $(SRCS:.c=.o)

LIBRARY = libmySimpleComputer.a

$(LIBRARY): $(OBJS)
	ar rcs $(LIBRARY) $(OBJS)

%.o: %.c $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean

clean:
	rm -f $(OBJS) $(LIBRARY)
