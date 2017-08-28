CC=gcc
FLAGS=--std=gnu11 -lncurses

build:
	@$(CC) src/*.c -o viw $(FLAGS)

run:
	@$(MAKE) build
	@./viw test.txt
	@rm viw

clean:
	rm -f viw *~

mem:
	@$(CC) $(FLAGS) src/*.c -o viw
	valgrind --leak-check=yes ./viw test.txt
	@rm viw

test-buffer:
	@$(MAKE) -C tests buffer

test-commands:
	@$(MAKE) -C tests commands

test-state:
	@$(MAKE) -C tests state
