CC = gcc
CFLAGS = -g -Wall -Wextra -Werror -I./include -pthread

# Директории
MY_SIMPLE_COMPUTER_DIR = mySimpleComputer
MY_TERM_DIR = myTerm
MY_BIGCHARS_DIR = myBigChars
MY_READKEY_DIR = myReadkey
CONSOLE_DIR = console

# Библиотеки
LIBS = -L$(MY_SIMPLE_COMPUTER_DIR) -lmySimpleComputer \
       -L$(MY_TERM_DIR) -lmyTerm \
       -L$(MY_BIGCHARS_DIR) -lmyBigChars \
       -L$(MY_READKEY_DIR) -lmyReadkey

# Цели
.PHONY: all clean $(MY_SIMPLE_COMPUTER_DIR) $(MY_TERM_DIR) $(MY_BIGCHARS_DIR) $(MY_READKEY_DIR) console

all: $(MY_SIMPLE_COMPUTER_DIR) $(MY_TERM_DIR) $(MY_BIGCHARS_DIR) $(MY_READKEY_DIR) console

# Сборка библиотек
$(MY_SIMPLE_COMPUTER_DIR):
	$(MAKE) -C $@

$(MY_TERM_DIR):
	$(MAKE) -C $@

$(MY_BIGCHARS_DIR):
	$(MAKE) -C $@

$(MY_READKEY_DIR):
	$(MAKE) -C $@

# Сборка консоли
console:
	$(MAKE) -C $(CONSOLE_DIR)

# Очистка
clean:
	$(MAKE) -C $(MY_SIMPLE_COMPUTER_DIR) clean
	$(MAKE) -C $(MY_TERM_DIR) clean
	$(MAKE) -C $(MY_BIGCHARS_DIR) clean
	$(MAKE) -C $(MY_READKEY_DIR) clean
	$(MAKE) -C $(CONSOLE_DIR) clean