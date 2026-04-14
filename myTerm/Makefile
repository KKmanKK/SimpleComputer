CC      = gcc
CFLAGS = -g -Wall -I../include

HEADER = myTerm.h

SRCS = myTerm.c
OBJS = $(SRCS:.c=.o)

LIBRARY = libmyTerm.a

.PHONY: all clean

all: $(LIBRARY)

$(LIBRARY): $(OBJS)
	ar rcs $(LIBRARY) $(OBJS)

%.o: %.c $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(LIBRARY)