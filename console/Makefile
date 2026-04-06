CC = gcc
CFLAGS = -g -Wall -Wextra -Werror -I../include -pthread
LDFLAGS = -L../mySimpleComputer -L../myTerm -L../myBigChars -L../myReadkey
LDLIBS = -lmySimpleComputer -lmyTerm -lmyBigChars -lmyReadkey -pthread

SRCS = console.c
OBJS = $(SRCS:.c=.o)
TARGET = console

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS) $(LDLIBS)

clean:
	rm -f $(OBJS) $(TARGET)

run: $(TARGET)
	./$(TARGET)