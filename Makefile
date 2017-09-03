CC=gcc
CFLAGS=
LDFLAGS=--std=gnu11 -lncurses

ifeq (Windows_NT, $(OS))
CFLAGS+=$(shell ncursesw6-config --cflags)
LDFLAGS=$(shell ncursesw6-config --libs)
endif

build:
	@$(CC) $(CFLAGS) src/*.c -o viw $(LDFLAGS)

run:
	@$(MAKE) build
	@./viw test.txt
	@rm viw

clean:
	rm -f viw *~

mem:
	@$(CC) $(CFLAGS) src/*.c -o viw $(LDFLAGS)
	valgrind --leak-check=yes ./viw test.txt
	@rm viw

test-buffer:
	@$(MAKE) -C tests buffer

test-commands:
	@$(MAKE) -C tests commands

test-state:
	@$(MAKE) -C tests state
