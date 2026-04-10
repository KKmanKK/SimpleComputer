CC     = gcc
CFLAGS = -g -Wall -I.

HEADER = mySimpleComputer.h

# Теперь только один файл с реализацией
SRCS = mySimpleComputer.c

OBJS = $(SRCS:.c=.o)

LIBRARY = libmySimpleComputer.a

# Сборка статической библиотеки
$(LIBRARY): $(OBJS)
	ar rcs $(LIBRARY) $(OBJS)

# Компиляция объектного файла
%.o: %.c $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@

# Пример программы для тестирования (опционально)
# TEST_SRCS = test.c mySimpleComputer.c
# TEST_TARGET = test
# 
# $(TEST_TARGET): $(TEST_SRCS) $(HEADER)
# 	$(CC) $(CFLAGS) $(TEST_SRCS) -o $(TEST_TARGET)

# Очистка
.PHONY: clean

clean:
	rm -f $(OBJS) $(LIBRARY)