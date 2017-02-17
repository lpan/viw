CC=gcc
FLAGS=--std=gnu11

run:
	@$(CC) $(FLAGS) src/*.c -o ez
	@./ez test.txt
	@rm ez

build:
	@$(CC) $(FLAGS) src/*.c -o ez

clean:
	rm -f ez *~

mem:
	@$(CC) $(FLAGS) src/*.c -o ez
	valgrind --leak-check=yes ./ez test.txt
	rm ez

test:
	@$(CC) $(FLAGS) src/state.c src/editor.c tests/state-test.c -o test
	valgrind --leak-check=yes ./test
	@rm test
