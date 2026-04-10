CC     = gcc
CFLAGS = -g -Wall -I../include

HEADER = ../include/myReadKey.h

SRCS = rk_variables.c \
       rk_mytermsave.c \
       rk_mytermrestore.c \
       rk_mytermregime.c \
       rk_readkey.c \
       rk_readvalue.c

OBJS    = $(SRCS:.c=.o)
LIBRARY = libmyReadKey.a

# Every object file depends on the header: changing myReadKey.h triggers full rebuild
%.o: %.c $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@

$(LIBRARY): $(OBJS)
	ar rcs $(LIBRARY) $(OBJS)

.PHONY: all clean

all: $(LIBRARY)

clean:
	rm -f $(OBJS) $(LIBRARY)
