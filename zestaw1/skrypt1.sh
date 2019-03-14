#!/bin/bash

rozmiar=$1
opcjaD=" -d "
opcjaF=" -f "
polecenie="./main -c "
kompilacja="make static"

polecenie="${polecenie}${rozmiar}"
rozmiar=$(( $rozmiar-1 ))

for i in `seq 0 $rozmiar`
do
        polecenie="${polecenie}${opcjaF} /home *.txt tmp.txt"
done


for i in `seq 0 $rozmiar`
do 
	polecenie="${polecenie}${opcjaD}${i}"
done

$kompilacja
$polecenie
