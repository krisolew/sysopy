all:
	gcc -c -Wall belt.c
	ar rcs belt.a belt.o
	gcc -c -Wall trucker.c
	gcc trucker.o belt.a -o trucker
	gcc -c -Wall loader.c
	gcc loader.o belt.a -o loader

clear:
	rm -f trucker
	rm -f loader
	rm -f *.[oa]
