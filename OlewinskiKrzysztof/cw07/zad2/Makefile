all:
	gcc -c -Wall belt.c -lpthread
	ar rcs belt.a belt.o
	gcc -c -Wall trucker.c -lpthread
	gcc trucker.o belt.a -o trucker -lpthread
	gcc -c -Wall loader.c -lpthread
	gcc loader.o belt.a -o loader -lpthread

clear:
	rm -f trucker
	rm -f loader
	rm -f *.[oa]
