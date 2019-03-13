static:
	gcc -c -Wall library.c
	gcc -c -Wall zadanie1.c
	gcc -Wall library.o zadanie1.o -o main

clear:
	rm -f *.o main