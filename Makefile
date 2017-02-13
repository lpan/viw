CC=gcc

ez: src/main.c
	$(CC) src/main.c -o ez

clean:
	rm -f ez *~
