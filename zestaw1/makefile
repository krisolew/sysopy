static:
	gcc -c -Wall library.c
	ar rcs library.a library.o
	gcc -c -Wall main.c
	gcc main.o library.a -o main

clear:
	rm -f *.o main
	rm -f *.so*
	rm -f tmp.txt

dynamic:
	gcc -Wall -fPIC -c library.c
	gcc -Wall -shared -Wl,-soname,liblibrary.so.1 -o liblibrary.so.1.0.1 library.o -lc
	ln -s liblibrary.so.1.0.1 liblibrary.so.1
	ln -s liblibrary.so.1 liblibrary.so

	gcc main.c -llibrary -o main -L. 

dll:
	gcc -Wall -fPIC -c library.c
	gcc -Wall -shared liblibrary.so library.o
	gcc main.c -llibrary -o main -L. -ldl -D DLL

