CC = gcc
CFLAGS = -g -Wall -I../include
LDFLAGS = -L../mySimpleComputer -L../myTerm -L../myBigChars -L../myReadkey
LIBS = -lmySimpleComputer -lmyTerm -lmyBigChars -lmyReadkey

TARGET = SimpleComputer

SRCS = $(wildcard *.c)

OBJS = $(SRCS:.c=.o)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f *.o
	
clean_all:
	rm -f *.o
	rm -f $(OBJ) $(TARGET)
