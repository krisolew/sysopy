#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <malloc.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>



void generate(int size, int number, char * name){
    if(creat(name,O_CREAT)==-1){
        printf("nie mozna utworzyc\n");
        return;
    };
    system("chmod 775 /home/anqe/pierwszy.txt");
    system("chmod 775 /home/anqe/from.txt");
    system("chmod 775 /home/anqe/to.txt");
    int fp = open(name, O_RDWR | O_CREAT);
    int j;
    int i;
    char * buf = calloc((size_t) size +1, sizeof(char));
    for(j= 0; j<number; j++) {
        for (i = 0; i < size; i++) {
            buf[i] = (char) (rand() % 26 + 'A');
        }
        buf[size] = '\n';
        if(write(fp, buf,((size +1) * sizeof(char)))==-1){
            printf("nie mozna zapisac");
        }
    }
}

void sort_sys(int size, char * name, int l_linii){
    int fp = open(name,  O_RDWR );
    unsigned char * buf = calloc((size_t) size +1, sizeof(char));
    unsigned char * minimum_buf = calloc((size_t) size +1, sizeof(char));
    int minimum_index = 0;
    int unsorted_index = 0;
    int i = 0;
    unsigned char minimum = 255;
    unsigned char current = 0;


    for(unsorted_index; unsorted_index < l_linii; unsorted_index++) {
        for (i = unsorted_index; i < l_linii; i++) {
            lseek(fp, i * (size + 1), SEEK_SET);
            read(fp, &current, sizeof(char));

            if (current < minimum) {
                minimum = current;
                minimum_index = i;
            }
        }

        lseek(fp, unsorted_index * (size +1), SEEK_SET);
        read(fp, buf, (size +1) *sizeof(char));

        lseek(fp, minimum_index * (size +1), SEEK_SET);
        read(fp, minimum_buf, (size +1) *sizeof(char));

        lseek(fp, unsorted_index * (size +1), SEEK_SET);
        write(fp, minimum_buf, (size +1) *sizeof(char));

        lseek(fp, minimum_index * (size +1), SEEK_SET);
        write(fp, buf, (size +1) *sizeof(char));

        minimum = 255;

    }
    close(fp);
    free(buf);
    free(minimum_buf);

}

void sort_lib(int size, char * name, int l_linii){
    FILE * fp = fopen(name,"r+");
    unsigned char * buf = calloc((size_t) size +1, sizeof(char));
    unsigned char * minimum_buf = calloc((size_t) size +1, sizeof(char));
    int minimum_index = 0;
    int unsorted_index = 0;
    int i = 0;
    unsigned char minimum = 255;
    unsigned char current = 0;


    for(unsorted_index; unsorted_index < l_linii; unsorted_index++) {
        for (i = unsorted_index; i < l_linii; i++) {
            fseek(fp, i * (size + 1), 0);
            fread(&current, sizeof(char), 1, fp);

            if (current < minimum) {
                minimum = current;
                minimum_index = i;
            }
        }

        fseek(fp, unsorted_index * (size + 1), 0);
        fread(buf, sizeof(char), size +1 , fp);

        fseek(fp, minimum_index * (size + 1), 0);
        fread(minimum_buf, sizeof(char), size +1, fp);

        fseek(fp, unsorted_index * (size +1), 0);
        fwrite(minimum_buf, sizeof(char), (size+1), fp);

        fseek(fp, minimum_index * (size +1), 0);
        fwrite(buf, sizeof(char), (size+1), fp);

        minimum = 255;

    }
    fclose(fp);
    free(buf);
    free(minimum_buf);
}

void copy_sys(int size, int bufer_size, char * from, char * to){
    int from1 = open(from, O_RDWR);
    int to1 = open(to,O_RDWR | O_APPEND);
    char * buf = calloc((size_t) bufer_size, sizeof(char));
    int i = 0;
    int stop = 1;
    for(; stop != 0; i++){
        stop = read(from1, buf, bufer_size * sizeof(char));
        int j = 0;
        for(; j < bufer_size; j++)
            printf("%c", buf[j]);
        printf("\n----------------\n");
        write(to1, buf, bufer_size * sizeof(char));
    }

}

int main() {
    srand(time(NULL));
    generate(20, 50, "/home/anqe/pierwszy.txt");
    generate(20, 50, "/home/anqe/from.txt");
    generate(20, 50, "/home/anqe/to.txt");
    //sort_sys(20, "/home/anqe/pierwszy.txt", 50);
    sort_lib(20, "/home/anqe/pierwszy.txt", 50);
    copy_sys(20*50, 20,"/home/anqe/from.txt", "/home/anqe/to.txt" );
    return 0;
}
