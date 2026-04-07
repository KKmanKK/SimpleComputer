# Компилятор и флаги
CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11 -g
LDFLAGS = 

# Директории
SRCDIR = .
OBJDIR = obj
BINDIR = bin

# Исходные файлы
SRCS = $(SRCDIR)/SimpleComputer.c \
       $(SRCDIR)/myTerm.c \
       $(SRCDIR)/myBigChars.c \
       $(SRCDIR)/myReadkey.c \
       $(SRCDIR)/myUI.c \
       $(SRCDIR)/main.c

# Объектные файлы
OBJS = $(SRCS:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

# Исполняемый файл
TARGET = $(BINDIR)/computer

# Цель по умолчанию
all: directories $(TARGET)

# Создание директорий
directories:
	@mkdir -p $(OBJDIR) $(BINDIR)

# Сборка исполняемого файла
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Компиляция .c файлов в .o
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Очистка объектных файлов
clean:
	rm -rf $(OBJDIR)/*.o $(TARGET)

# Полная очистка
distclean: clean
	rm -rf $(OBJDIR) $(BINDIR)

# Запуск программы
run: all
	./$(TARGET)

# Отладка
debug: CFLAGS += -DDEBUG -g
debug: all
	gdb $(TARGET)

# Проверка с Valgrind
valgrind: all
	valgrind --leak-check=full --track-origins=yes ./$(TARGET)

# Справка
help:
	@echo "Доступные цели:"
	@echo "  make all       - собрать проект"
	@echo "  make run       - собрать и запустить"
	@echo "  make clean     - удалить объектные файлы"
	@echo "  make distclean - полная очистка"
	@echo "  make debug     - собрать с отладочной информацией"
	@echo "  make valgrind  - проверить утечки памяти"
	@echo "  make help      - показать эту справку"

.PHONY: all clean distclean run debug valgrind help directories