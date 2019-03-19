#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <malloc.h>
#include <stdlib.h>
#include <time.h>


void generate(int size, int number, char * name){
    if(creat(name,O_CREAT)==-1){
        printf("nie mozna utworzyc\n");
        return;
    };
    system("chmod 775 /home/pierwszy.txt");
    int fp = open(name, O_RDWR | O_CREAT);
    int j;
    int i;
    int * buf = calloc((size_t) size +1, sizeof(int));
    for(j= 0; j<number; j++) {
        for (i = 0; i < size; i++) {
            buf[i] =  rand() % 26 + 'A';
        }
        buf[size] = '\n';
        if(write(fp, buf,((size +1) * sizeof(int)))==-1){
            printf("nie mozna zapisac");
        }
    }
}

void sort_sys(int size, char * name){
    int fp = open(name, O_RDONLY);
    unsigned char * buf = calloc((size_t) size +1, sizeof(int));
    read(fp, buf, ((size +1) * sizeof(int)));
    for(int i = 0; i <= size*4; i++) {
        printf("%c", buf[i]);
    }
    int min_index = 0;
    for(int i = 0; i < size; i++){
    }
}

void sort_lib(){

}

void copy_sys(){

}

int main() {
    srand(time(NULL));
    generate(20, 3, "/home/pierwszy.txt");
    sort_sys(20, "/home/pierwszy.txt");
    return 0;
}
