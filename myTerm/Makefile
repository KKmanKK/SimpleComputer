CC     = gcc
CFLAGS = -g -Wall -I.

HEADER = myTerm.h

# Основной файл с реализацией
SRCS = myTerm.c
OBJS = $(SRCS:.c=.o)
LIBRARY = libmyTerm.a

# Тестовая программа
TEST_SRCS = main.c
TEST_TARGET = test

# Сборка статической библиотеки
$(LIBRARY): $(OBJS)
	ar rcs $(LIBRARY) $(OBJS)

# Компиляция объектного файла
%.o: %.c $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@

# Сборка тестовой программы с библиотекой
$(TEST_TARGET): $(LIBRARY) $(TEST_SRCS)
	$(CC) $(CFLAGS) $(TEST_SRCS) -L. -lmyTerm -o $(TEST_TARGET)

# Альтернативный способ: собрать без библиотеки
# $(TEST_TARGET): $(TEST_SRCS) $(SRCS) $(HEADER)
# 	$(CC) $(CFLAGS) $(TEST_SRCS) $(SRCS) -o $(TEST_TARGET)

# Очистка
.PHONY: clean all

all: $(LIBRARY) $(TEST_TARGET)

clean:
	rm -f $(OBJS) $(LIBRARY) $(TEST_TARGET)