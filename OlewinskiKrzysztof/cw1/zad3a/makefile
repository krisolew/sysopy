static: clear
	gcc -c -Wall library.c
	ar rcs library.a library.o
	gcc -c -Wall main.c
	gcc main.o library.a -o main
	make test

clear:
	rm -f *.o main
	rm -f *.so*
	rm -f tmp.txt

shared: clear
	export LD_LIBRARY_PATH=.:$SD_LIBRARY_PATH
	gcc -Wall -fPIC -c library.c
	gcc -Wall -shared -Wl,-soname,liblibrary.so.1 -o liblibrary.so.1.0.1 library.o -lc
	ln -s liblibrary.so.1.0.1 liblibrary.so.1
	ln -s liblibrary.so.1 liblibrary.so
	gcc main.c -llibrary -o main -L. 
	make test

dll: clear 
	export LD_LIBRARY_PATH=.:$SD_LIBRARY_PATH
	gcc -Wall -fPIC -c library.c
	gcc -Wall -shared -o liblibrary.so library.o
	gcc main.c -llibrary -o main -L. -ldl -D DLL
	make test

test:
	./main -c 2000000 -f /home a*.txt tmp.txt
	./main -c 2000 -f /bin a*.txt tmp.txt
	./main -c 200000 -f /etc c*.txt tmp.txt 
	./main -c 2000000 -f /home a*.txt tmp.txt -d 0
	./main -c 2000 -f /bin a*.txt tmp.txt -d 0
	./main -c 200000 -f /etc c*.txt tmp.txt  -d 0
	./main -c 30 -f /home c*.txt tmp.txt -d 0 -f /home c*.txt tmp.txt -d 0 -f /home c*.txt tmp.txt -d 0 -f /home c*.txt tmp.txt -d 0 -f /home c*.txt tmp.txt -d 0 -f /home c*.txt tmp.txt -d 0 -f /home c*.txt tmp.txt -d 0 -f /home c*.txt tmp.txt -d 0 -f /home c*.txt tmp.txt -d 0 -f /home c*.txt tmp.txt -d 0 -f /home c*.txt tmp.txt -d 0 -f /home c*.txt tmp.txt -d 0

