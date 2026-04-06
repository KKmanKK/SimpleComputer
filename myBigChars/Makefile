CC = gcc
CFLAGS = -g -Wall -Wextra -Werror -I../include
AR = ar
ARFLAGS = rcs

SRCS = myBigChars.c
OBJS = $(SRCS:.c=.o)
LIBRARY = libmyBigChars.a

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(LIBRARY): $(OBJS)
	$(AR) $(ARFLAGS) $@ $^

clean:
	rm -f $(OBJS) $(LIBRARY)