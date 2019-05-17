all:
	gcc -c -Wall belt.c -lrt
	ar rcs belt.a belt.o
	gcc -c -Wall trucker.c -lrt
	gcc trucker.o belt.a -o trucker -lrt
	gcc -c -Wall loader.c -lrt
	gcc loader.o belt.a -o loader -lrt

clear:
	rm -f trucker
	rm -f loader
	rm -f *.[oa]
