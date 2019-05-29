all:
	gcc -c -Wall belt.c -lpthread-lrt
	ar rcs belt.a belt.o
	gcc -c -Wall trucker.c -lpthread -lrt
	gcc trucker.o belt.a -o trucker -lpthread -lrt
	gcc -c -Wall loader.c -lpthread -lrt
	gcc loader.o belt.a -o loader -lpthread -lrt

clear:
	rm -f trucker
	rm -f loader
	rm -f *.[oa]
