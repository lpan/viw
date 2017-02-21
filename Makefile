CC=gcc
FLAGS=--std=gnu11 -lncurses

run:
	@$(CC) $(FLAGS) src/*.c -o viw
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

test:
	@$(CC) $(FLAGS) src/buffer.c tests/buffer-test.c -o test
	valgrind --leak-check=yes ./test
	@rm test
