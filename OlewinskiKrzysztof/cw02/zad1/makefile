all:
	gcc -Wall main.c -o main

test: all
	rm -f wyniki.txt 
	./main -g test.txt 2000 1 
	cp test.txt test1.txt
	./main -s test.txt 2000 1 sys -s test1.txt 2000 1 lib

	./main -g test.txt 2000 4 
	cp test.txt test1.txt
	./main -s test.txt 2000 4 sys -s test1.txt 2000 4 lib

	./main -g test.txt 2000 512
	cp test.txt test1.txt
	./main -s test.txt 2000 512 sys -s test1.txt 2000 512 lib

	./main -g test.txt 2000 1024
	cp test.txt test1.txt
	./main -s test.txt 2000 1024 sys -s test1.txt 2000 1024 lib

	./main -g test.txt 2000 4096 
	cp test.txt test1.txt
	./main -s test.txt 2000 4096 sys -s test1.txt 2000 4096 lib

	./main -g test.txt 2000 8192
	cp test.txt test1.txt
	./main -s test.txt 2000 8192 sys -s test1.txt 2000 8192 lib

	./main -g test.txt 5000 1
	cp test.txt test1.txt
	./main -s test.txt 5000 1 sys -s test1.txt 5000 1 lib

	./main -g test.txt 5000 4 
	cp test.txt test1.txt
	./main -s test.txt 5000 4 sys -s test1.txt 5000 4 lib

	./main -g test.txt 5000 512 
	cp test.txt test1.txt
	./main -s test.txt 5000 512 sys -s test1.txt 5000 512 lib

	./main -g test.txt 5000 1024 
	cp test.txt test1.txt
	./main -s test.txt 5000 1024 sys -s test1.txt 5000 1024 lib

	./main -g test.txt 5000 4096 
	cp test.txt test1.txt
	./main -s test.txt 5000 4096 sys -s test1.txt 5000 4096 lib

	./main -g test.txt 5000 8192 
	cp test.txt test1.txt
	./main -s test.txt 5000 8192 sys -s test1.txt 5000 8192 lib

	./main -g test.txt 10000 1
	cp test.txt test1.txt
	touch test2.txt
	./main -c test.txt test2.txt 10000 1 sys -c test1.txt test2.txt 10000 1 lib
	rm test2.txt

	./main -g test.txt 10000 4 
	cp test.txt test1.txt
	touch test2.txt
	./main -c test.txt test2.txt 10000 4 sys -c test1.txt test2.txt 10000 4 lib
	rm test2.txt

	./main -g test.txt 10000 512 
	cp test.txt test1.txt
	touch test2.txt
	./main -c test.txt test2.txt 10000 512 sys -c test1.txt test2.txt 10000 512 lib
	rm test2.txt

	./main -g test.txt 10000 1024 
	cp test.txt test1.txt
	touch test2.txt
	./main -c test.txt test2.txt 10000 1024 sys -c test1.txt test2.txt 10000 1024 lib
	rm test2.txt

	./main -g test.txt 10000 4096 
	cp test.txt test1.txt
	touch test2.txt
	./main -c test.txt test2.txt 10000 4096 sys -c test1.txt test2.txt 10000 4096 lib
	rm test2.txt

	./main -g test.txt 10000 8192 
	cp test.txt test1.txt
	touch test2.txt
	./main -c test.txt test2.txt 10000 8192 sys -c test1.txt test2.txt 10000 8192 lib

	./main -g test.txt 50000 1
	cp test.txt test1.txt
	touch test2.txt
	./main -c test.txt test2.txt 50000 1 sys -c test1.txt test2.txt 50000 1 lib
	rm test2.txt

	./main -g test.txt 50000 4 
	cp test.txt test1.txt
	touch test2.txt
	./main -c test.txt test2.txt 50000 4 sys -c test1.txt test2.txt 50000 4 lib
	rm test2.txt

	./main -g test.txt 50000 512 
	cp test.txt test1.txt
	touch test2.txt
	./main -c test.txt test2.txt 50000 512 sys -c test1.txt test2.txt 50000 512 lib
	rm test2.txt

	./main -g test.txt 50000 1024 
	cp test.txt test1.txt
	touch test2.txt
	./main -c test.txt test2.txt 50000 1024 sys -c test1.txt test2.txt 50000 1024 lib
	rm test2.txt

	./main -g test.txt 50000 4096 
	cp test.txt test1.txt
	touch test2.txt
	./main -c test.txt test2.txt 50000 4096 sys -c test1.txt test2.txt 50000 4096 lib
	rm test2.txt

	./main -g test.txt 50000 8192 
	cp test.txt test1.txt
	touch test2.txt
	./main -c test.txt test2.txt 50000 8192 sys -c test1.txt test2.txt 50000 8192 lib
	make clear

clear:
	rm test.txt
	rm test1.txt
	rm test2.txt