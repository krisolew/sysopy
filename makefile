static:
	gcc -c -Wall library.c
	ar rcs library.a library.o
	gcc -c -Wall zadanie1.c
	gcc zadanie1.o library.a -o main

clear:
	rm -f *.o main
	rm -f *.so*

dynamic:
	gcc -Wall -fPIC -c library.c
	gcc -Wall -shared -Wl,-soname,liblibrary.so.1 -o liblibrary.so.1.0.1 library.o -lc
	ln -s liblibrary.so.1.0.1 liblibrary.so.1
	ln -s liblibrary.so.1 liblibrary.so

	gcc zadanie1.c -llibrary -o main -L. 

dll:
	gcc -Wall -fPIC -c library.c
	gcc -Wall -shared liblibrary.so library.o
	gcc zadanie1.c -llibrary -o main -L. -ldl -D DLL

