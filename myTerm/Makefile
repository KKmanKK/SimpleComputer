CC = gcc
CFLAGS = -g -Wall -Wextra -Werror -I../include
AR = ar
ARFLAGS = rcs

SRCS = myTerm.c
OBJS = $(SRCS:.c=.o)
LIBRARY = libmyTerm.a

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(LIBRARY): $(OBJS)
	$(AR) $(ARFLAGS) $@ $^

clean:
	rm -f $(OBJS) $(LIBRARY)