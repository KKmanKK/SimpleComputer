CC      = gcc
CFLAGS = -g -Wall -I../include

HEADER = myReadKey.h

SRCS = myReadKey.c
OBJS = $(SRCS:.c=.o)

LIBRARY = libmyReadKey.a

.PHONY: all clean

all: $(LIBRARY)

$(LIBRARY): $(OBJS)
	ar rcs $(LIBRARY) $(OBJS)

%.o: %.c $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(LIBRARY)