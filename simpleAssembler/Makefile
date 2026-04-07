CC = gcc

TARGET = simpleAssembler

SRC = main.c

CFLAGS = -std=c11

all: $(TARGET)

$(TARGET): $(SRC)
		$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(TARGET)