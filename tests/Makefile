CC=gcc
FLAGS=--std=gnu11 -lncurses

buffer:
	@$(CC) $(FLAGS) ../src/buffer.c ./buffer-test.c -o test
	valgrind --leak-check=yes ./test
	@rm test
