CC=gcc
FLAGS=--std=c99

run: src/*.c
	@$(CC) $(FLAGS) src/*.c -o ez
	@./ez test.txt

clean:
	rm -f ez *~
