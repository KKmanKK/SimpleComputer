CC      = gcc
CFLAGS  = -g -Wall -I../include -I.
LDFLAGS = -L../mySimpleComputer -lmySimpleComputer \
          -L../myTerm -lmyTerm \
          -L../myBigChars -lmyBigChars \
          -L../myReadKey -lmyReadKey

SC_LIB   = ../mySimpleComputer/libmySimpleComputer.a
TERM_LIB = ../myTerm/libmyTerm.a
BC_LIB   = ../myBigChars/libmyBigChars.a
RK_LIB   = ../myReadKey/libmyReadKey.a
LIBS     = $(SC_LIB) $(TERM_LIB) $(BC_LIB) $(RK_LIB)

# Все исходные файлы консоли
CONSOLE_SRCS = console.c ALU.c CU.c IRC.c
CONSOLE_OBJS = $(CONSOLE_SRCS:.c=.o)

.PHONY: all test run clean FORCE

all: test console font sum factorial
	

# Принудительная пересборка библиотек
$(SC_LIB): FORCE
	@$(MAKE) -C ../mySimpleComputer

$(TERM_LIB): FORCE
	@$(MAKE) -C ../myTerm

$(BC_LIB): FORCE
	@$(MAKE) -C ../myBigChars

$(RK_LIB): FORCE
	@$(MAKE) -C ../myReadKey

# Сборка консоли
console: main.o $(CONSOLE_OBJS) $(LIBS)
	$(CC) $(CFLAGS) main.o $(CONSOLE_OBJS) $(LDFLAGS) -o console



# Общее правило для компиляции .c файлов
%.o: %.c console.h
	$(CC) $(CFLAGS) -c $< -o $@

# Генератор шрифта
font: font.o $(BC_LIB)
	$(CC) $(CFLAGS) font.o -L../myBigChars -lmyBigChars -o font

# Генераторы бинарных файлов
sum: sum.c
	$(CC) $(CFLAGS) sum.c -o sum

factorial: factorial.c
	$(CC) $(CFLAGS) factorial.c -o factorial

# Запуск
run: console font
	@./font
	@./console

font-run: console font
	@./font
	@./console

# Очистка
clean:
	@rm -f console main.o $(CONSOLE_OBJS) test_memory.bin font font.o sum sum.o factorial factorial.o
	@$(MAKE) -C ../mySimpleComputer clean
	@$(MAKE) -C ../myTerm clean
	@$(MAKE) -C ../myBigChars clean
	@$(MAKE) -C ../myReadKey clean

FORCE: