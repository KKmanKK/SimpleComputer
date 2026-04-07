CC = gcc
CFLAGS = -g -Wall -I../include

SRCS = myTerm.c

OBJS = $(SRCS:.c=.o)

LIBRARY = libmyTerm.a

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(LIBRARY): $(OBJS)
	ar rcs $(LIBRARY) $(OBJS)

.PHONY: clean

clean:
	rm -f $(OBJS) $(LIBRARY)
