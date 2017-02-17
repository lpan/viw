CC=gcc
FLAGS=--std=c11

run:
	@$(CC) $(FLAGS) src/*.c -o ez
	@./ez test.txt
	@rm ez

build:
	@$(CC) $(FLAGS) src/*.c -o ez

clean:
	rm -f ez *~
