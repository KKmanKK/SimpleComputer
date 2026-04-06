CC = gcc
CFLAGS = -g -Wall -Wextra -Werror -I../include
AR = ar
ARFLAGS = rcs

SRCS = myReadkey.c
OBJS = $(SRCS:.c=.o)
LIBRARY = libmyReadkey.a

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(LIBRARY): $(OBJS)
	$(AR) $(ARFLAGS) $@ $^

clean:
	rm -f $(OBJS) $(LIBRARY)