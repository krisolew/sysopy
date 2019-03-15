
static: clear
	gcc -c -Wall library.c
	ar rcs library.a library.o


clear:
	rm -f *.o 
	rm -f *.so*
	rm -f *.a

shared: clear
	gcc -Wall -fPIC -c library.c
	gcc -Wall -shared -Wl,-soname,liblibrary.so.1 -o liblibrary.so.1.0.1 library.o -lc
	ln -s liblibrary.so.1.0.1 liblibrary.so.1
	ln -s liblibrary.so.1 liblibrary.so
	 
	


	



