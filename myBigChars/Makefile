CC      = gcc
CFLAGS = -g -Wall -I../include

HEADER = myBigChars.h

SRCS = myBigChars.c
OBJS = $(SRCS:.c=.o)

LIBRARY = libmyBigChars.a

.PHONY: all clean

all: $(LIBRARY)

$(LIBRARY): $(OBJS)
	ar rcs $(LIBRARY) $(OBJS)

%.o: %.c $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(LIBRARY)