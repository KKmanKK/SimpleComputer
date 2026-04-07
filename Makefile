CC = gcc
CFLAGS = -g

.PHONY: all clean

TARGET = SimpleComputer

all: mySimpleComputer.a myTerm.a myBigChars.a myReadkey.a SimpleComputer clean

mySimpleComputer.a:
	cd mySimpleComputer && $(MAKE)

myTerm.a:
	cd myTerm && $(MAKE)

myBigChars.a:
	cd myBigChars && $(MAKE)

myReadkey.a:
	cd myReadkey && $(MAKE)

SimpleComputer:
	cd console && $(MAKE)

clean:
	cd mySimpleComputer && $(MAKE) clean
	cd myTerm && $(MAKE) clean
	cd myBigChars && $(MAKE) clean
	cd myReadkey && $(MAKE) clean
	cd console && $(MAKE) clean
	rm -f *.o

clean_all:
	cd mySimpleComputer && $(MAKE) clean
	cd myTerm && $(MAKE) clean
	cd myBigChars && $(MAKE) clean
	cd myReadkey && $(MAKE) clean
	cd console && $(MAKE) clean_all
	rm -f *.o
