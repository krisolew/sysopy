
type=0

static: clear
	gcc -c -O$(type) -Wall library.c
	ar rcs library.a library.o
	gcc -c -O$(type) -Wall main.c
	gcc main.o library.a -o main
	make test

clear:
	rm -f *.o main
	rm -f *.so*
	rm -f *.a
	rm -f tmp.txt

shared: clear
	export LD_LIBRARY_PATH=.:$SD_LIBRARY_PATH
	gcc -Wall -fPIC -O$(type) -c library.c 
	gcc -shared -o library.so library.o
	gcc main.c -O$(type) library.o -o main -L. 
	make test

dll: clear 
	gcc -Wall -fPIC -c -O$(type) library.c
	gcc -Wall -shared -o liblibrary.so library.o
	gcc main.c -O$(type) -llibrary -o main -L. -ldl -D DLL
	make test

test:
	./main -c 2000000 -f /home a*.txt tmp.txt
	./main -c 2000 -f /bin a*.txt tmp.txt
	./main -c 200000 -f /etc c*.txt tmp.txt 
	./main -c 2000000 -f /home a*.txt tmp.txt -d 0
	./main -c 2000 -f /bin a*.txt tmp.txt -d 0
	./main -c 200000 -f /etc c*.txt tmp.txt  -d 0
	./main -c 30 -f /home c*.txt tmp.txt -d 0 -f /home c*.txt tmp.txt -d 0 -f /home c*.txt tmp.txt -d 0 -f /home c*.txt tmp.txt -d 0 -f /home c*.txt tmp.txt -d 0 -f /home c*.txt tmp.txt -d 0 -f /home c*.txt tmp.txt -d 0 -f /home c*.txt tmp.txt -d 0 -f /home c*.txt tmp.txt -d 0 -f /home c*.txt tmp.txt -d 0 -f /home c*.txt tmp.txt -d 0 -f /home c*.txt tmp.txt -d 0

