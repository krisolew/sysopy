all:
	gcc -c -Wall belt.c
	ar rcs belt.a belt.o
	gcc -c -Wall trucker.c
	gcc trucker.o belt.a -o trucker

clear:
	rm -f trucker
