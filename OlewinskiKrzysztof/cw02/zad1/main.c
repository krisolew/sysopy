#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <fcntl.h>
#include <malloc.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#define PLIK "wyniki.txt"

static struct tms st_cpu;
static struct tms en_cpu;

void measure_start_time()
{
    times(&st_cpu);
}

void measure_stop_time()
{
    times(&en_cpu);
}

void print_and_save_times(char parametr)
{
    FILE* file = fopen(PLIK,"a+");

    switch(parametr){
        case 'g':
            printf("wywołanie funkcji generate: \n");
            fprintf(file, "wywołanie funkcji generate \n");
            break;
        case 's':
            printf("wywołanie funkcji sort: \n");
            fprintf(file, "wywołanie funkcji sort: \n");
            break;
        case 'c':
            printf("wywołanie funkcji copy: \n");
            fprintf(file, "wywołanie funkcji copy: \n");
            break;
    }


    long double cpu_user_time = (long double)(en_cpu.tms_utime - st_cpu.tms_utime)/sysconf(_SC_CLK_TCK);
    long double cpu_system_time = (long double)(en_cpu.tms_stime - st_cpu.tms_stime)/sysconf(_SC_CLK_TCK);

    printf("Cpu user time %Lf s\n", cpu_user_time);
    printf("Cpu system time %Lf s\n", cpu_system_time);
    printf("\n");

    fprintf(file, "Cpu user time %Lf s\n", cpu_user_time);
    fprintf(file, "Cpu system time %Lf s\n", cpu_system_time);
    fprintf(file, "\n");
    fclose(file);
}







void generate(int size, int number, char * name){
    if(creat(name,O_CREAT)==-1)
    {
        printf("nie mozna utworzyc\n");
        return;
    }

    char *command = malloc(200*sizeof(char));
    strcpy(command, "chmod 775 ");
    strcat(command, name);
    system(command);
    
    int fp = open(name, O_RDWR | O_CREAT);
    int j;
    int i;
    char * buf = calloc((size_t) size, sizeof(char));
    for(j= 0; j<number; j++) {
        for (i = 0; i < size; i++) {
            buf[i] = (char) (rand() % 26 + 'A');
        }
        if(write(fp, buf,((size) * sizeof(char)))==-1){
            printf("nie mozna zapisac");
        }
    }
}

void sort_sys(int size, char * name, int l_linii){
    int fp = open(name,  O_RDWR );
    unsigned char * buf = calloc((size_t) size, sizeof(char));
    unsigned char * minimum_buf = calloc((size_t) size, sizeof(char));
    int minimum_index = 0;
    int unsorted_index = 0;
    int i = 0;
    unsigned char minimum = 255;
    unsigned char current = 0;


    for(; unsorted_index < l_linii; unsorted_index++) {
        for (i = unsorted_index; i < l_linii; i++) {
            lseek(fp, i * (size), SEEK_SET);
            read(fp, &current, sizeof(char));

            if (current < minimum) {
                minimum = current;
                minimum_index = i;
            }
        }
        if(minimum_index!=unsorted_index){

            lseek(fp, unsorted_index * (size), SEEK_SET);
            read(fp, buf, (size) *sizeof(char));

            lseek(fp, minimum_index * (size), SEEK_SET);
            read(fp, minimum_buf, (size) *sizeof(char));

            lseek(fp, unsorted_index * (size), SEEK_SET);
            write(fp, minimum_buf, (size) *sizeof(char));

            lseek(fp, minimum_index * (size), SEEK_SET);
            write(fp, buf, (size) *sizeof(char));
        }

        minimum = 255;

    }
    close(fp);
    free(buf);
    free(minimum_buf);

}

void sort_lib(int size, char * name, int l_linii){
    FILE * fp = fopen(name,"r+");
    unsigned char * buf = calloc((size_t) size, sizeof(char));
    unsigned char * minimum_buf = calloc((size_t) size, sizeof(char));
    int minimum_index = 0;
    int unsorted_index = 0;
    int i = 0;
    unsigned char minimum = 255;
    unsigned char current = 0;


    for(; unsorted_index < l_linii; unsorted_index++) {
        for (i = unsorted_index; i < l_linii; i++) {
            fseek(fp, i * (size), 0);
            fread(&current, sizeof(char), 1, fp);

            if (current < minimum) {
                minimum = current;
                minimum_index = i;
            }
        }
        if(minimum_index!=unsorted_index){

            fseek(fp, unsorted_index * (size), 0);
            fread(buf, sizeof(char), size , fp);

            fseek(fp, minimum_index * (size), 0);
            fread(minimum_buf, sizeof(char), size, fp);

            fseek(fp, unsorted_index * (size), 0);
            fwrite(minimum_buf, sizeof(char), (size), fp);

            fseek(fp, minimum_index * (size), 0);
            fwrite(buf, sizeof(char), (size), fp);
        }

        minimum = 255;

    }
    fclose(fp);
    free(buf);
    free(minimum_buf);
}

void copy_sys(int size, int buffer_size, char * from, char * to){
    
    int fp = open(from, O_RDONLY);
    int res = lseek(fp, 0, SEEK_END);
    close(fp);

    if(res<size) size = res;

    int from1 = open(from, O_RDWR);
    int to1 = open(to,O_RDWR | O_APPEND);
    char * buf = calloc((size_t) buffer_size, sizeof(char));
    int i = 0;
    while (i < (size - buffer_size) )
    {
        read(from1, buf, buffer_size * sizeof(char));
        write(to1, buf, buffer_size * sizeof(char));
        i+=buffer_size;
    }
    read(from1, buf, (size-i) * sizeof(char));
    write(to1, buf, (size-i) * sizeof(char));

    close(from1);
    close(to1);
}

void copy_lib(int size, int buffer_size, char * from, char * to){

    FILE* fp = fopen(from, "r");
    fseek(fp, 0, SEEK_END);
    int res = ftell(fp);
    fclose(fp);

    if(res<size) size = res;

    FILE *from1 = fopen(from, "r");
    FILE *to1 = fopen(to, "a+");
    char * buf = calloc((size_t) buffer_size, sizeof(char));
    int i = 0;
    while (i < (size - buffer_size) )
    {
        fread(buf, sizeof(char), buffer_size, from1);
        fwrite(buf, sizeof(char), buffer_size, to1);
        i+=buffer_size;
    }
    fread(buf, sizeof(char), size-i, from1);
    fwrite(buf, sizeof(char), size-i, to1);

    fclose(from1);
    fclose(to1);
}

int main(int argc, char* argv[]) {
    srand(time(NULL));
    int block_size = 20;
    int number_of_blocks = 50;
    int buffer_size = 14;
    char * plik1;
    char * plik2;
    char * type;
    

    

    int opt;
    int index = 1;
    int i=0;

    while ((opt=getopt(argc,argv,"g:s:c:")) != -1)
    {
        index++;
        switch(opt)
        {
            case 'g':

                plik1 = strdup(argv[index++]);
                number_of_blocks = 0;
                while (argv[index][i]!='\0')
                {
                    number_of_blocks*=10;
                    number_of_blocks += (int) argv[index][i] - '0';
                    i++;
                }

                i=0;
                index++;
                block_size = 0;
                while (argv[index][i]!='\0')
                {
                    block_size*=10;
                    block_size += (int) argv[index][i] - '0';
                    i++;
                }
                

                measure_start_time();
                generate(block_size, number_of_blocks, plik1);
                measure_stop_time();

                FILE* file1 = fopen(PLIK,"a");
                fprintf(file1, "polecenie: ");
                fprintf(file1, " -g %s %d %d ", plik1, number_of_blocks, block_size);
                fprintf(file1, "\n");
                fclose(file1);

                print_and_save_times(opt);
                break;

            case 's':

                plik1 = strdup(argv[index++]);
                number_of_blocks = 0;
                while (argv[index][i]!='\0')
                {
                    number_of_blocks*=10;
                    number_of_blocks += (int) argv[index][i] - '0';
                    i++;
                }

                index++;
                i=0;
                block_size = 0;
                while (argv[index][i]!='\0')
                {
                    block_size*=10;
                    block_size += (int) argv[index][i] - '0';
                    i++;
                }

                type = strdup(argv[++index]);

                measure_start_time();
                if (strcmp(type, "sys")==0) sort_sys(block_size, plik1, number_of_blocks);
                else if (strcmp(type, "lib")==0) sort_lib(block_size, plik1, number_of_blocks);
                else{
                    printf("wrong type argument %s\n", type);
                    return -1;
                }
                measure_stop_time();

                file1 = fopen(PLIK,"a");
                fprintf(file1, "polecenie: ");
                fprintf(file1, " -s %s %d %d %s ", plik1, number_of_blocks, block_size, type);
                fprintf(file1, "\n");
                fclose(file1);

                print_and_save_times(opt);

                break;

            case 'c':
                plik1 = strdup(argv[index++]);
                plik2 = strdup(argv[index++]);
                number_of_blocks = 0;
                while (argv[index][i]!='\0')
                {
                    number_of_blocks*=10;
                    number_of_blocks += (int) argv[index][i] - '0';
                    i++;
                }

                i=0;
                index++;
                block_size = 0;
                while (argv[index][i]!='\0')
                {
                    block_size*=10;
                    block_size += (int) argv[index][i] - '0';
                    i++;
                }

                type = strdup(argv[++index]);

                measure_start_time();
                if (strcmp(type, "sys")==0) copy_sys(block_size*number_of_blocks, buffer_size, plik1, plik2 );
                else if (strcmp(type, "lib")==0) copy_lib(block_size*number_of_blocks, buffer_size, plik1, plik2);
                else{
                    printf("wrong type argument %s\n", type);
                    return -1;
                }
                measure_stop_time();

                file1 = fopen(PLIK,"a");
                fprintf(file1, "polecenie: ");
                fprintf(file1, " -c %s %s %d %d %s ", plik1, plik2, number_of_blocks, block_size, type);
                fprintf(file1, "\n");
                fclose(file1);

                print_and_save_times(opt);
                break;
        }
        index++;
        i=0;
    }

    return 0;
}
