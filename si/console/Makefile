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

CONSOLE_SRCS = variables.c drawBoxes.c \
               printCell.c printFlags.c printDecodedCommand.c \
               printAccumulator.c printCounters.c \
               printTerm.c printCommand.c printBigChars.c printBigCell.c \
               moveCursor.c editInPlace.c \
               refreshConsole.c ALU.c CU.c IRC.c
CONSOLE_OBJS = $(CONSOLE_SRCS:.c=.o)

.PHONY: all test run clean FORCE

all: test console font
	@./test

# FORCE ensures sub-makes always run to check for library changes
$(SC_LIB): FORCE
	@$(MAKE) -C ../mySimpleComputer

$(TERM_LIB): FORCE
	@$(MAKE) -C ../myTerm

$(BC_LIB): FORCE
	@$(MAKE) -C ../myBigChars

$(RK_LIB): FORCE
	@$(MAKE) -C ../myReadKey

# console rebuilds when: any library changes OR any console source/header changes
console: main.o $(CONSOLE_OBJS) $(LIBS)
	$(CC) $(CFLAGS) main.o $(CONSOLE_OBJS) $(LDFLAGS) -o console

test: test.o $(CONSOLE_OBJS) $(LIBS)
	$(CC) $(CFLAGS) test.o $(CONSOLE_OBJS) $(LDFLAGS) -o test

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# font is a standalone tool: only needs libmyBigChars
font: font.o $(BC_LIB)
	$(CC) $(CFLAGS) font.o -L../myBigChars -lmyBigChars -o font

run: console font
	@./font
	@./console

# Called from the top-level Makefile's "run" target (all libs already built)
font-run: console font
	@./font
	@./console

clean:
	@rm -f console main.o test test.o $(CONSOLE_OBJS) test_memory.bin font font.o
	@$(MAKE) -C ../mySimpleComputer clean
	@$(MAKE) -C ../myTerm clean
	@$(MAKE) -C ../myBigChars clean
	@$(MAKE) -C ../myReadKey clean
