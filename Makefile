CC=gcc
FLAGS=--std=gnu11 -lncurses

run:
	@$(MAKE) build
	@./viw test.txt
	@rm viw

build:
	@$(CC) $(FLAGS) src/*.c -o viw

clean:
	rm -f viw *~

mem:
	@$(CC) $(FLAGS) src/*.c -o viw
	valgrind --leak-check=yes ./viw test.txt
	@rm viw

test-buffer:
	@$(MAKE) -C tests buffer

test-state:
	@$(MAKE) -C tests state
