CC = gcc
CFLAGS = -g -Wall -I../include

SRCS = mySimpleComputer.c

OBJS = $(SRCS:.c=.o)

LIBRARY = libmySimpleComputer.a

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(LIBRARY): $(OBJS)
	ar rcs $(LIBRARY) $(OBJS)

.PHONY: clean

clean:
	rm -f $(OBJS) $(LIBRARY)
	rm -f *.o
