CC     = gcc
CFLAGS = -g -Wall -I../include

HEADER = ../include/mySimpleComputer.h

SRCS = sc_variables.c \
       sc_memoryInit.c sc_memorySet.c sc_memoryGet.c sc_memorySave.c sc_memoryLoad.c \
       sc_regInit.c sc_regSet.c sc_regGet.c \
       sc_accumulatorInit.c sc_accumulatorSet.c sc_accumulatorGet.c \
       sc_icounterInit.c sc_icounterSet.c sc_icounterGet.c \
       sc_commandEncode.c sc_commandDecode.c sc_commandValidate.c

OBJS = $(SRCS:.c=.o)

LIBRARY = libmySimpleComputer.a

$(LIBRARY): $(OBJS)
	ar rcs $(LIBRARY) $(OBJS)

%.o: %.c $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean

clean:
	rm -f $(OBJS) $(LIBRARY)
